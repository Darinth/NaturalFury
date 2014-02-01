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
	GameState gameState;
	int height;
	int width;
	list<shared_ptr<EngineMsg>> msgQueue;
	list<shared_ptr<GameView>> viewList;
	multimap<unsigned int, shared_ptr<Process>> processList;
	unsigned int currentTick;

	GameEngine(const GameEngine& gameEngine) = delete;
	GameEngine& operator =(const GameEngine& gameEngine) = delete;
	
public:
	GameEngine();
	virtual ~GameEngine();

	virtual void tick();
	virtual void sendMsg(const shared_ptr<EngineMsg>& msg);
	void addView(shared_ptr<GameView> view);
	void addProcess(shared_ptr<Process> process);
	GameState getGameState();
};

#endif
