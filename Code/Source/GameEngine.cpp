// Name:
// GameEngine.cpp
// Description:
// Implementation file for GameEngine class.
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "GameEngine.h"

#include <thread>
#include <sstream>
using namespace std;

#include "Globals.h"
#include "GameView.h"
#include "Process.h"
#include "EngineMsg.h"
#include "Logger.h"

GameEngine::GameEngine() : gameState(GameState::INITIALIZING), currentTick(0), engineThread(nullptr) {}

GameEngine::~GameEngine()
{
	if (gameState != GameState::SHUT_DOWN)
	{
		globalLogger->eWriteLog("GameEngine deconstructed without being shut down!", LogLevel::Error, {});
	}

	//Make sure that the engineThread ends before we destroy the GameEngine
	if (engineThread)
	{
		engineThread->join();
		delete engineThread;
	}
}

void GameEngine::tick()
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Don't do anything if the engine has been shut down.
	if (gameState == GameState::SHUT_DOWN)
		return;

	//Loop through the processes until we find one that doesn't need to run yet or we hit the end of the list
	multimap<unsigned int, shared_ptr<Process>>::iterator it = processList.begin();
	while (it != processList.end() && it->first < currentTick)
	{
		//Get the process, remove it from the list, run it, and insert the resultant process (often the same process, set for a future tick) back into the list.
		shared_ptr<Process> process = it->second;
		it = processList.erase(it);
		process = process->trigger();
		if (process)
			processList.insert(multimap<unsigned int, shared_ptr<Process>>::value_type(process->triggerTick, process));
	}
}

void GameEngine::sendMsg(const shared_ptr<EngineMsg>& msg)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//If this is a shutdown message, shut down.
	if (msg->msgType == MsgTypeEnum::Shutdown)
	{
		gameState = GameState::SHUT_DOWN;
	}

	//Push msgs in to queue.
	msgQueue.push_back(msg);

	//Push msgs to views. Currently disabled until message filtering is enabled.
	for (list<shared_ptr<GameView>>::iterator it = viewList.begin(); it != viewList.end(); it++)
	{
		//(**it).sendMsg(msg);
	}
}

void GameEngine::addView(shared_ptr<GameView> view)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Add view to view list.
	viewList.push_back(view);
}

void GameEngine::addProcess(shared_ptr<Process> process)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Add process to process list
	processList.insert(multimap<unsigned int, shared_ptr<Process>>::value_type(process->triggerTick, process));
}

GameState GameEngine::getGameState()
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Return the current game state
	return gameState;
}

void GameEngine::start()
{
	//Start the thread if it hasn't been started, throw an error if it's already been started.
	if (!engineThread)
		engineThread = new thread(&GameEngine::engineLoop, this);
	else
		throw exception("Attempt to start game engine that is already running...");
}

const chrono::milliseconds tickLength(16);

void GameEngine::engineLoop()
{
	//Schedule the next tick to happen immediately
	chrono::steady_clock::time_point nextTick = chrono::steady_clock::now();

	//Variables for tracking how far behind the engine has gotten
	long long ticksBehind;
	bool runningBehind = false;
	
	//Set the game state to running
	gameState = GameState::RUNNING;

	//Log entry into engine loop
	globalLogger->eWriteLog("In GameEngine::engineLoop", LogLevel::Info, {});

	//Run until the GameEngine gets shut down
	while (gameState != GameState::SHUT_DOWN)
	{
		//If we're ready for the next tick, run the tick
		if (chrono::steady_clock::now() > nextTick)
		{
			//Schedule the next tick
			nextTick += tickLength;
			//Proceses a tick
			tick();
			this_thread::yield();
		}
		//If we're not ready for the next tick, sleep for a bit
		else
		{
			auto timeToNextTick = nextTick - chrono::steady_clock::now();
			this_thread::sleep_for(timeToNextTick);
		}

		//Calculate the number of ticks we are behind, and if we're more than 30 ticks behind log a message.
		chrono::milliseconds timeDiff = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - nextTick);
		ticksBehind = timeDiff.count() / tickLength.count();
		if (ticksBehind >= 30 && !runningBehind)
		{
			runningBehind = true;
			globalLogger->eWriteLog("WARNING: Processor has fallen behind the tick rate!", LogLevel::Warning, {});
		}
		//Also log a message once we've caught back up.
		else if (ticksBehind < 5 && runningBehind)
		{
			runningBehind = false;
			globalLogger->eWriteLog("WARNING: Processor has caught back up with the tick rate!", LogLevel::Warning, {});
		}
	}
	//Log a message for the engine exiting it's loop
	globalLogger->eWriteLog("Finished with GameEngine::engineLoop", LogLevel::Info, {});
}
