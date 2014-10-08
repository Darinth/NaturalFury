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
		gameWindow.shiftDebugCamera(glm::dvec3(0.0, 0.0, 1.0));
	else if (key == KeyEnum::A)
		gameWindow.shiftDebugCamera(glm::dvec3(-1.0, 0.0, 0.0));
	else if (key == KeyEnum::S)
		gameWindow.shiftDebugCamera(glm::dvec3(0.0, 0.0, -1.0));
	else if (key == KeyEnum::D)
		gameWindow.shiftDebugCamera(glm::dvec3(1.0, 0.0, 0.0));
	else if (key == KeyEnum::E)
		gameWindow.updateScene();
	else if (key == KeyEnum::Q)
		gameWindow.toggleAutoUpdate();
}

void PlayerView::keyUp(KeyEnum key)
{

}

void PlayerView::mouseMove(long distanceX, long distanceY)
{
	gameWindow.rotateDebugCamera(distanceX);
	gameWindow.tiltDebugCamera(distanceY);
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
