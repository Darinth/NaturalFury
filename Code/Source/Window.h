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

class PlayerView;
class GraphicsEngine;
class CubeModel;
class ShaderProgram;
class Scene;

class Window
{
private:
	//Hidden Data. Uses reinterpret_cast in Window.cpp to get access to actual data.
	struct Pimpl;
	Pimpl* pimpl;
	PlayerView* playerView;
	string className;
	bool fullScreen;
	int width, height;
	int bits;
	GraphicsEngine *graphicsEngine;
	Window(const Window& window);
	Window(Window&& window);
	Window operator =(const Window& window);
	bool keyStates[(unsigned short)KeyEnum::TotalKeys];

	//Debug variables.
	glm::dvec3 debugCameraPosition;
	double debugCameraTilt;
	double debugCameraRotation;
	ShaderProgram *shader;
	shared_ptr<CubeModel> *cubeModel;
	Scene *scene;
	bool autoUpdate;

public:
	Window(string title, string className, bool fullScreen, int width, int height, int bits, PlayerView* playerView);
	~Window();
	void shiftDebugCamera(glm::dvec3 shift);
	void rotateDebugCamera(double angle);
	void tiltDebugCamera(double angle);
	void updateScene();
	void toggleAutoUpdate();

#ifdef _WINDOWS
	virtual int handleMessage(void* windowHandle, unsigned int message, unsigned int wParam, long lParam);
#endif

	void show();
};

#endif