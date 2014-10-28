// Name:
// InterfaceThread.h
// Description:
// Header file for InterfaceThread class.
// Notes:
// OS-Unaware
// On construction, starts a thread to to the interface. Done indicates when the thread is finished.

#ifndef INTERFACE_THREAD_H
#define INTERFACE_THREAD_H

#include <thread>
using namespace std;

class Window;

class InterfaceThread
{
private:
	thread *interfaceThread;
	Window *window;
	void threadLoop();
public:
	bool done;
	InterfaceThread(Window *window);
	~InterfaceThread();
};

#endif
