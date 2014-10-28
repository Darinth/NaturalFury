// Name:
// InterfaceThread.cpp
// Description:
// Implementation file for InterfaceThread class
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "InterfaceThread.h"

#include "GameEngine.h"
#include "PlayerView.h"
#include "GraphicsEngine.h"

InterfaceThread::InterfaceThread(Window *window) : done(false), window(window), interfaceThread(nullptr)
{
	window->getGraphicsEngine()->relinquish();
	interfaceThread = new thread(&InterfaceThread::threadLoop, this);
}

InterfaceThread::~InterfaceThread()
{
	interfaceThread->join();
	delete interfaceThread;
	window->getGraphicsEngine()->claim();
}

void InterfaceThread::threadLoop()
{
	window->getGraphicsEngine()->claim();
	GameEngine *gameEngine = new GameEngine();
	PlayerView *playerView = new PlayerView();
	gameEngine->start();
	playerView->run(window, gameEngine);
	delete playerView;
	delete gameEngine;
	
	window->getGraphicsEngine()->relinquish();
	this->done = true;
}