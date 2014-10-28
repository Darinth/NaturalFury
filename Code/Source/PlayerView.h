// Name:
// PlayerView.h
// Description:
// Header file for PlayerView.h
// PlayerView is a virtual class used as the base for local and network player views.
// Notes:
// OS-Unaware

#ifndef PLAYER_VIEW_H
#define PLAYER_VIEW_H

#include "GameView.h"
#include "Window.h"
#include "KeyEnum.h"
#include "UserInterface.h"

class GraphicsEngine;

class PlayerView : public GameView, public UserInterface
{
private:
	Window* gameWindow;
	GraphicsEngine* graphicsEngine;

	PlayerView(const PlayerView&) = delete;
	PlayerView& operator=(const PlayerView*) = delete;
	bool done;
	bool shiftDown;

	//Debug variables.
	glm::dvec3 debugCameraPosition;
	double debugCameraTilt;
	double debugCameraRotation;
	glm::vec3 sunColor;

public:
	PlayerView();
	virtual ~PlayerView();

	//Messages sent by the window
	virtual void keyDown(KeyEnum key);
	virtual void keyUp(KeyEnum key);
	virtual void mouseMove(long distanceX, long distanceY);
	virtual void mouseDown(int button);
	virtual void mouseUp(int button);
	virtual void gamePadButtonDown(int number);
	virtual void gamePadButtonUp(int number);
	virtual void gamePadThrottleChange(int number, double value);
	virtual void windowClose();
	//virtual void sendMsg(const shared_ptr<EngineMsg>& msg) = 0;
	
	//Tells the PlayerView to begin a closed loop, this function does not return until the application is finished.
	virtual void run(Window* gameWindow, GameEngine* gameEngine);

	//Debug camera code
	void shiftDebugCamera(glm::dvec3 shift);
	void rotateDebugCamera(double angle);
	void tiltDebugCamera(double angle);
	void updateScene(Scene* scene);
};

#endif