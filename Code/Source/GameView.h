// Name:
// GameView.h
// Description:
// Header file for GameView class.
// GameView is an abstract class designed to be the view of the game for a particular subsystem such as AI, the player view, network views, etc...
// Notes:
// OS-Unaware

#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "Lockable.h"

namespace std
{
	template<class _Ty>
	class shared_ptr;
}
class EngineMsg;

class GameView : public Lockable
{
private:
	GameView(const GameView& gameView) = delete;
	GameView& operator=(const GameView& gameView) = delete;

public:
	const bool requiresProcessing;

public:
	GameView();
	virtual ~GameView();

	virtual void sendMsg(const shared_ptr<EngineMsg>& msg) = 0;
	virtual void process();
};

#endif