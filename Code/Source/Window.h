// Name:
// Window.h
// Description:
// Header file for Window class.
// The Window class is an OS-Agnostic view of a window that is responsible for setting up a window and providing an interface for dealing with it that doesn't change from OS to OS.
// Notes:
// OS-Aware
// File does not pollute with OS-Headers, is only aware for one define.

#ifndef WINDOW_H
#define WINDOW_H

#include <memory>

#include <glm/glm.hpp>

#include "Vec3.h"
#include "KeyEnum.h"

class UserInterface;
class GraphicsEngine;
class CubeModel;
class ShaderProgram;
class Scene;

class Window
{
private:
	//Pimpl idiom to hide windows types from the rest of the application. Uses reinterpret_cast in Window.cpp to get access to actual data.
	struct Pimpl;
	Pimpl* pimpl;
	//UserInterface to send keystrokes, etc... to.
	UserInterface* userInterface;
	//Used by some OSes for window creation
	string className;
	//Whether the window was started in fullscreen mode.
	bool fullScreen;
	//Width/height of the window
	int width, height;
	//Color bit-depth
	int bits;
	//GraphicsEngine used by application to draw to this window
	GraphicsEngine *graphicsEngine;
	//Disable copy construction
	Window(const Window& window) = delete;
	Window operator =(const Window& window) = delete;
	//Key states, to prevent key repeats.
	bool keyStates[(unsigned short)KeyEnum::TotalKeys];

public:
	Window(string title, string className, bool fullScreen, int width, int height, int bits);
	~Window();
	//Return the window's graphicsEngine
	GraphicsEngine* getGraphicsEngine();

	//Sets the user interface to send UI events to.
	void setUserInterface(UserInterface* userInterface);
	//Returns the user interface that UI events are being sent to.
	UserInterface* getUserInterface();

#ifdef _WINDOWS
	//Used by MS windows OS to send messages to the window
	virtual int handleMessage(void* windowHandle, unsigned int message, unsigned int wParam, long lParam);
#endif

	//Show the window.
	void show();
};

#endif