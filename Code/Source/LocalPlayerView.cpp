// Name:
// LocalPlayerView.cpp
// Description:
// Implementation file for LocalPlayerView class
// Notes:
// OS-Unaware

#include "LocalPlayerView.h"

#include "GameEngine.h"
#include "EngineMsg.h"

extern GameEngine *gameEngine;

LocalPlayerView::LocalPlayerView() : PlayerView()//("Game Engine", "GameEngine", false, 800, 450, 32)
{

}

LocalPlayerView::~LocalPlayerView()
{

}

void LocalPlayerView::sendMsg(const shared_ptr<EngineMsg>& msg)
{

}

void LocalPlayerView::windowClosed()
{
	//On window close, tell the engine to shut down.
	gameEngine->pushMsg(shared_ptr<EngineMsg>(new EngineMsgShutdown()));
}
