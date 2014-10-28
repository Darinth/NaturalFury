// Name:
// GameEngine.h
// Description:
// Header file for GameEngine class
// GameEngine is the central class in Natural Fury. It is used to pass messages back and forth between components.
// Notes:
// OS-Unaware

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <thread>
#include <list>
#include <map>
#include <memory>
using namespace std;

#include "EngineMsg.h"
#include "Lockable.h"

class GameView;
class Process;

enum class GameState
{
	INITIALIZING,
	RUNNING,
	SHUT_DOWN
};

class GameEngine : public Lockable
{
private:
	//Variable to monitor the current state of the engine.
	GameState gameState;
	//Messages going to the GameEngine, typically from views
	list<shared_ptr<EngineMsg>> msgQueue;
	//List of attached views
	list<shared_ptr<GameView>> viewList;
	//List of currently running processes
	multimap<unsigned int, shared_ptr<Process>> processList;
	//Tracks current engine tick for process management
	unsigned int currentTick;

	//pointer to thread that hosts the engineLoop
	thread* engineThread;

	//Disable copy construction
	GameEngine(const GameEngine& gameEngine) = delete;
	GameEngine& operator =(const GameEngine& gameEngine) = delete;

	//Thread that runs the game loop
	void engineLoop();
	
public:
	GameEngine();
	virtual ~GameEngine();

	//Perform a tick
	virtual void tick();
	//Send a message to the engine
	virtual void sendMsg(const shared_ptr<EngineMsg>& msg);
	//Add a view
	void addView(shared_ptr<GameView> view);
	//Add a process
	void addProcess(shared_ptr<Process> process);
	//Retrieve current game state
	GameState getGameState();
	//Start up the GameEngine thread, call only once.
	void start();
};

#endif
