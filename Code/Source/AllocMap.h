// Name:
// AllocMap.h
// Description:
// Header file for AllocMap class
// AllocMap is a hashmap designed to track the memory allocations of the new and delete functions and thus does not use the c++ new/delete functions itself.
// Notes:
// OS-Unaware

#ifndef ALLOC_MAP_H
#define ALLOC_MAP_H

#include <string>
using namespace std;

//Data for the allocation of a single pointer
struct AllocData
{
	void* ptr;
	const char *sourceFile;
	const char *funcName;
	unsigned int lineNum;
};

class AllocMap
{
private:
	hash<void*> hasher;
	AllocData  *allocDataMap;
	unsigned long allocatedSlots;
	unsigned long usedSlots;
	unsigned long reallocationPoint;
	short shift;

	void doubleSlots();
public:
	typedef AllocData *iterator;

public:
	AllocMap();
	~AllocMap();
	void map(const AllocData& allocData);
	AllocData retrieve(void* ptr);
	void erase(void* ptr);
	AllocMap::iterator begin();
	AllocMap::iterator end();
};

#endif
