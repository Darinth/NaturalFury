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

GameEngine::GameEngine()
{
	//Set our state
	gameState = GameState::INITIALIZING;
	height = 0;
	width = 0;
}

GameEngine::~GameEngine()
{

}

void GameEngine::tick()
{
	lock_guard<recursive_mutex> objectLock(objectMutex);
	//Process all of the views
	for (list<shared_ptr<GameView>>::iterator it = viewList.begin(); it != viewList.end(); it++)
	{
		(**it).process();
	}
	//Process the physics engine
	//Process any other events
}

void GameEngine::pushMsg(const shared_ptr<EngineMsg>& msg)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);
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
