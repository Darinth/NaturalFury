// Name:
// UserInterface.h
// Description:
// Header file for UserInterface.h
// Virtual class to stsandardize how anything handles user input and displays things to the screen.
// Notes:
// OS-Unaware

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "KeyEnum.h"

class GameEngine;
class Window;

class UserInterface
{
public:
	virtual void keyDown(KeyEnum key) = 0;
	virtual void keyUp(KeyEnum key) = 0;
	virtual void mouseMove(long distanceX, long distanceY) = 0;
	virtual void mouseDown(int button) = 0;
	virtual void mouseUp(int button) = 0;
	virtual void gamePadButtonDown(int number) = 0;
	virtual void gamePadButtonUp(int number) = 0;
	virtual void gamePadThrottleChange(int number, double value) = 0;
	virtual void windowClose() = 0;
	virtual void run(Window* gameWindow, GameEngine* gameEngine) = 0;
};


#endif
