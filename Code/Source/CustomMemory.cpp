#define _CRT_SECURE_NO_WARNINGS

#include <cstddef>
#include <iostream>
#include <exception>
#include <string>
#include <functional>
#include <mutex>
using namespace std;

#include "AllocMap.h"

recursive_mutex memoryMutex;

//Allocation map to keep track of where everything is allocated to/from
AllocMap allocMap;

//Variable used to temporarily disable allocation tracking while running getAllocs();
bool allocDisabled = false;

//Forward declarations
void NF_delete(void* ptr);
void* operator new(unsigned int size, const char* sourceFile, const char *funcName, unsigned int lineNum);
void* operator new[](unsigned int size, const char* sourceFile, const char *funcName, unsigned int lineNum);

string getAllocs()
{
	lock_guard<recursive_mutex> memoryLock(memoryMutex);
	//If we're running without allocation tracking, don't return anything.
	if (allocDisabled == true)
		return string();
	//Allocate memory to hold results
	char temp[5000];
	//Disable memory allocation tracking within the block
	allocDisabled = true;
	{
		strcpy(temp, allocMap.getAllocData().substr(0, 4999).c_str());
	}
	//Re-enable allocation tracking
	allocDisabled = false;
	//Allocate a string and return the results.
	return string(temp);
}

void* operator new(unsigned int size)
{
	return operator new(size, "Unknown", "Unknown", 0);
}

void* operator new[](unsigned int size)
{
	return operator new[](size, "Unknown", "Unknown", 0);
}

void* operator new(unsigned int size, const char* sourceFile, const char *funcName, unsigned int lineNum)
{
	//Allocate memory for pointer
	void* temp = malloc(size);
	//Unless allocation tracking is disabled, track allocated memory.
	{
		lock_guard<recursive_mutex> memoryLock(memoryMutex);
		if (!allocDisabled)
			allocMap.map(AllocData{ temp, sourceFile, funcName, lineNum });
	}
	//Return pointer
	return temp;
}

void* operator new[](unsigned int size, const char* sourceFile, const char *funcName, unsigned int lineNum)
{
	//Allocate memory for pointer
	void* temp = malloc(size);
	//Unless allocation tracking is disabled, track allocated memory.
	{
		lock_guard<recursive_mutex> memoryLock(memoryMutex);
		if (!allocDisabled)
			allocMap.map(AllocData{ temp, sourceFile, funcName, lineNum });
	}
	//Return pointer
	return temp;
}

void operator delete(void* ptr)
{
	//Forward to universal delete function
	NF_delete(ptr);
}

void operator delete[](void* ptr)
{
	//Forward to universal delete function
	NF_delete(ptr);
}

void operator delete(void* ptr, const char* sourceFile, const char *funcName, unsigned int lineNum)
{
	//Forward to universal delete function
	NF_delete(ptr);
}

void operator delete[](void* ptr, const char* sourceFile, const char *funcName, unsigned int lineNum)
{
	//Forward to universal delete function
	NF_delete(ptr);
}

void NF_delete(void* ptr)
{
	//Unless allocation tracking is disabled, remove tracked pointer
	{
		lock_guard<recursive_mutex> memoryLock(memoryMutex);
		if (!allocDisabled)
			allocMap.erase(ptr);
	}
	//Free the pointer
	free(ptr);
}
