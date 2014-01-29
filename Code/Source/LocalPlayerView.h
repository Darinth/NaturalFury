// Name:
// LocalPlayerView.h
// Description:
// Header file for LocalPlayerView class.
// LocalPlayerView represents a local player's view of the world. It creates a window, receives all of the input from that window, and is responsible for drawing to that window.
// Notes:
// OS-Unaware

#ifndef LOCAL_PLAYER_VIEW_H
#define LOCAL_PLAYER_VIEW_H

#include "PlayerView.h"

class LocalPlayerView : public PlayerView
{
private:
	LocalPlayerView(const LocalPlayerView&) = delete;
	LocalPlayerView& operator=(const LocalPlayerView*) = delete;
public:
	LocalPlayerView();
	virtual ~LocalPlayerView();

	virtual void windowClosed();
	virtual void sendMsg(const shared_ptr<EngineMsg>& msg);
};

#endif