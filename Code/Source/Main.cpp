// Name:
// Main.cpp
// Description:
// Contains entry point and main program loop
// Notes:
// OS-Aware

#include "CustomMemory.h"

#include <memory>
#include <future>
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
#include "PlayerView.h"
#include "InterfaceThread.h"

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

	//Initialize Logger
	globalLogger = new Logger("LogInit.xml", "General.log");

	//Initialize resource source
	MasterDirectoryResourceSource *mdrs = new MasterDirectoryResourceSource(".");
	mdrs->open();

	//Initialize resourec cache using resource source
	globalResourceCache = new ResourceCache(1024 * 1024 * 100, mdrs);
	globalResourceCache->registerProcessor(shared_ptr<IResourceProcessor>(new StringResourceProcessor));

	//Open application window
	Window *appWindow = new Window("Game Engine", "GameEngine", false, 800, 450, 32);

	//Start thread to handle application UI, this thread actually handles the bulk of the application work.
	InterfaceThread *interfaceThread = new InterfaceThread(appWindow);

	/*      Main Message loop*/

	//Until the application is done
	while(GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (interfaceThread->done)
			PostQuitMessage(0);
	}

	delete interfaceThread;
	delete appWindow;
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
