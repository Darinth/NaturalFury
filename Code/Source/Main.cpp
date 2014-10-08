// Name:
// Main.cpp
// Description:
// Contains entry point and main program loop
// Notes:
// OS-Aware

#include "CustomMemory.h"

#include <memory>

#include <Windows.h>
#include <string>
using namespace std;

#include "Globals.h"
#include "GameEngine.h"
#include "LocalPlayerView.h"
#include "Logger.h"
#include "MasterDirectoryResourceSource.h"
#include "ResourceCache.h"
#include "StringResourceProcessor.h"

/*      Screen/display attributes*/
int width = 800;
int height = 450;
int bits = 32;

HDC deviceContext;

GameEngine *gameEngine;

//Default Initialization values
bool fullScreen = false;

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{	
	MSG             msg;                    //message
	bool    done;                   //flag for completion of app

	globalLogger = new Logger("LogInit.xml", "General.log");

	MasterDirectoryResourceSource *mdrs = new MasterDirectoryResourceSource(".");

	mdrs->open();

	globalResourceCache = new ResourceCache(1024 * 1024 * 100, mdrs);
	globalResourceCache->registerProcessor(shared_ptr<IResourceProcessor>(new StringResourceProcessor));

	//Create GameEngnie
	gameEngine = new GameEngine();
	gameEngine->addView(shared_ptr<GameView>(new LocalPlayerView()));

	done = false;   //initialize loop condition variable

	//Set the next tick to happen immediately
	clock_t nextTick = clock();

	//variable to track number of ticks processed per game loop
	char ticksThisLoop;

	/*      Main program loop*/

	//Until the application is done
	while (!done)
	{
		//process events from the eventPipeline
		//Reset ticks this loop
		ticksThisLoop = 0;
		//While the clock is less than the time for the next tick and we've processed less than 30 ticks this game loop...
		while (clock() > nextTick && ticksThisLoop < 30)
		{
			//Increase tick count
			ticksThisLoop++;
			//Process a tick every 16 miliseconds (about 60/second)
			nextTick += 16;
			//Proceses a tick
			gameEngine->tick();
			if (gameEngine->getGameState() == GameState::SHUT_DOWN)
				PostQuitMessage(0);
			this_thread::yield();
		}

		//If we exited the tick loop due to having maxed out the tick counter, write a warning to the log.
		if(ticksThisLoop >= 30)
		{
			globalLogger->eWriteLog("WARNING: Processor is having trouble keeping up with game rate.", LogLevel::Warning, {});
		}

		//While there are messages to be processed...
		while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE) && !done)
		{
			//If the message is a quit message...
			if (msg.message == WM_QUIT)     //did we receive a quit message?
			{
				//Set done to true and get out, nothing else needs to be done
				done = true;
			}
			//Otherwise, pass the message to the message handler and get rid of it
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	delete gameEngine;
	delete globalLogger;
	delete globalResourceCache;
	delete mdrs;

	string allocLog = getAllocs();

	if (allocLog.length() > 0)
	{
		staticLog("AllocLog.log", allocLog, LogLevel::Error);
	}

	//return the quit message parameter
	return msg.wParam;
}
