// Name:
// Lockable.h
// Description:
// Header file for Lockable class.
// Simple class that defines a recursive mutex. Inherited by other classes that need to be lockable.
// Notes:
// OS-Unaware

#ifndef LOCKABLE_H
#define LOCKABLE_H

#include <mutex>
using namespace std;

class Lockable
{
public:
	mutable recursive_mutex objectMutex;
};

#endif