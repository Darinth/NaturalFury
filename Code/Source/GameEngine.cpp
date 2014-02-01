// Name:
// GameEngine.cpp
// Description:
// Implementation file for GameEngine class.
// Notes:
// OS-Unaware

#include "GameEngine.h"

#include <thread>
using namespace std;

#include "GameView.h"
#include "Process.h"
#include "EngineMsg.h"

GameEngine::GameEngine()
{
	//Set our state
	gameState = GameState::INITIALIZING;
	height = 0;
	width = 0;
	currentTick = 0;
}

GameEngine::~GameEngine()
{

}

void GameEngine::tick()
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	if (gameState == GameState::SHUT_DOWN)
		return;

	multimap<unsigned int, shared_ptr<Process>>::iterator it = processList.begin();
	while (it != processList.end() && it->first < currentTick)
	{
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
	if (msg->msgType == MsgTypeEnum::Shutdown)
	{
		gameState = GameState::SHUT_DOWN;
	}
	//Push msgs in to queue.
	msgQueue.push_back(msg);
	//Push msgs to views. Later on, get the messages to filter based upon view.
	for (list<shared_ptr<GameView>>::iterator it = viewList.begin(); it != viewList.end(); it++)
	{
		(**it).sendMsg(msg);
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

	processList.insert(multimap<unsigned int, shared_ptr<Process>>::value_type(process->triggerTick, process));
}

GameState GameEngine::getGameState()
{
	return gameState;
}
