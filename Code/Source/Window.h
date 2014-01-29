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

class PlayerView;

class Window
{
private:
	//Hidden Data. Uses reinterpret_cast in Window.cpp to get access to actual data.
	struct HData { char data[92]; };
	HData hData;
	PlayerView* playerView;
	string className;
	bool fullScreen;
	int width, height;
	int bits;
	Window(const Window& window);
	Window(Window&& window);
	Window operator =(const Window& window);

public:
	Window(string title, string className, bool fullScreen, int width, int height, int bits, PlayerView* playerView);
	~Window();

#ifdef _WIN32
	virtual int handleMessage(void* windowHandle, unsigned int message, unsigned int wParam, long lParam);
#endif

	void show();
};

#endif