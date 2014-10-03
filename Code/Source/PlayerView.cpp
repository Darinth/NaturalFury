// Name:
// PlayerView.cpp
// Description:
// Implementation Class for PlayerView class
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "PlayerView.h"
#include "Vec3.h"

PlayerView::PlayerView() : gameWindow("Game Engine", "GameEngine", false, 800, 450, 32, this)
{
	gameWindow.show();
}

PlayerView::~PlayerView()
{

}

void PlayerView::keyDown(KeyEnum key)
{
	if (key == KeyEnum::W)
		gameWindow.shiftDebugCamera(Vec3<double>(0.0, 0.0, 1.0));
	else if (key == KeyEnum::A)
		gameWindow.shiftDebugCamera(Vec3<double>(-1.0, 0.0, 0.0));
	else if (key == KeyEnum::S)
		gameWindow.shiftDebugCamera(Vec3<double>(0.0, 0.0, -1.0));
	else if (key == KeyEnum::D)
		gameWindow.shiftDebugCamera(Vec3<double>(1.0, 0.0, 0.0));
}

void PlayerView::keyUp(KeyEnum key)
{

}

void PlayerView::mouseMove(long distanceX, long distanceY)
{
	gameWindow.rotateDebugCamera(distanceX, Vec3<double>(0.0, 1.0, 0.0));
	gameWindow.rotateDebugCamera(distanceY, Vec3<double>(1.0, 0.0, 0.0));
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
