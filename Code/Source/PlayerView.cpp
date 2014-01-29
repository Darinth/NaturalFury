// Name:
// PlayerView.cpp
// Description:
// Implementation Class for PlayerView class
// Notes:
// OS-Unaware

#include "PlayerView.h"

PlayerView::PlayerView() : gameWindow("Game Engine", "GameEngine", false, 800, 450, 32, this)
{
	gameWindow.show();
}

PlayerView::~PlayerView()
{

}

void PlayerView::keyDown(KeyEnum key)
{

}

void PlayerView::keyUp(KeyEnum key)
{

}

void PlayerView::mouseMove(long distanceX, long distanceY)
{

}

void PlayerView::mouseClick(int button)
{

}

void PlayerView::gamePadButtonDown(int number)
{

}

void PlayerView::gamePadButtonUp(int number)
{

}

void PlayerView::gamePadThrottleChange(int number, double value)
{

}
