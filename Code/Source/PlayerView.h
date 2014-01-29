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

class PlayerView : public GameView
{
private:
	Window gameWindow;
	PlayerView(const PlayerView&) = delete;
	PlayerView& operator=(const PlayerView*) = delete;
public:
	PlayerView();
	virtual ~PlayerView();

	virtual void keyDown(KeyEnum key);
	virtual void keyUp(KeyEnum key);
	virtual void mouseMove(long distanceX, long distanceY);
	virtual void mouseClick(int button);
	virtual void gamePadButtonDown(int number);
	virtual void gamePadButtonUp(int number);
	virtual void gamePadThrottleChange(int number, double value);
	virtual void windowClosed() = 0;
	virtual void sendMsg(const shared_ptr<EngineMsg>& msg) = 0;
};

#endif