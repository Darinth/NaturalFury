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
#include <memory>
using namespace std;

#include "EngineMsg.h"
#include "Lockable.h"

class GameView;

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

	GameEngine(const GameEngine& gameEngine) = delete;
	GameEngine& operator =(const GameEngine& gameEngine) = delete;
	
public:
	GameEngine();
	virtual ~GameEngine();

	virtual void tick();
	virtual void pushMsg(const shared_ptr<EngineMsg>& msg);
	void addView(shared_ptr<GameView> view);
};

#endif