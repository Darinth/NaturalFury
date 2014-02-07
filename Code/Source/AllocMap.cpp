// Name:
// AllocMap.cpp
// Description:
// Implementation file for AllocMap class.
// Notes:
// OS-Unaware

#include "AllocMap.h"

#include <functional>
using namespace std;

AllocMap::AllocMap()
{
	//Make sure that unsigned int is 4 byte so bit shifting works properly
	static_assert(sizeof(unsigned int) == 4, "unsigned int size mismatch.");
	//Set allocation map to 'half' of what I want the defaults
	allocDataMap = nullptr;
	allocatedSlots = 256;
	shift = 24;
	usedSlots = 0;
	//Call doubleSlots which will allocate memory for the map and 512 slots.
	doubleSlots();
}

AllocMap::~AllocMap()
{
	//If the map was allocated, deallocate it.
	if (allocDataMap != nullptr)
		free(allocDataMap);
}

void AllocMap::doubleSlots()
{
	//Store old slot count, update slot count and shift.
	unsigned int oldSlotCount = allocatedSlots;
	allocatedSlots *= 2;
	shift--;
	//TODO: Check to see if /4*3 is faster or slower than *0.75 cast to int
	reallocationPoint = allocatedSlots/4*3;

	AllocData *oldMap = allocDataMap;

	//Allocate room for hash map
	allocDataMap = reinterpret_cast<AllocData*>(malloc(sizeof(AllocData)* allocatedSlots));

	//Set all of the pointer values to null
	for (unsigned int I = 0; I < allocatedSlots; I++)
	{
		allocDataMap[I].ptr = nullptr;
	}

	//If we have an old map
	if (oldMap != nullptr)
	{
		//Iterate through the old map's values and copy over any used slots
		for (unsigned int I = 0; I < oldSlotCount; I++)
		{
			if (oldMap[I].ptr != nullptr)
			{
				map(oldMap[I]);
			}
		}

		//Free the old map's memory
		free(oldMap);
	}
}

void AllocMap::map(const AllocData& allocData)
{
	//Increment used slots, this function shouldn't ever be called unsuccessfully
	usedSlots++;

	//If the number of used slots has grown to far, get more slots
	if (usedSlots >= reallocationPoint)
		doubleSlots();

	//Get base slot that this pointer will go to
	unsigned long slot = hasher(allocData.ptr) >> shift;

	//Get first empty slot following base slot
	while (allocDataMap[slot].ptr != nullptr)
	{
		if (allocDataMap[slot].ptr != allocData.ptr)
			slot++;
		else
			throw exception("Attempt to map already mapped pointer!");
		if (slot == allocatedSlots)
			slot = 0;
	}

	//Put allocation data in empty slot.
	allocDataMap[slot] = allocData;
}

AllocData AllocMap::retrieve(void* ptr)
{
	//Get starting slot for search
	unsigned long slot = hasher(ptr) >> shift;
	//Store starting slot to make sure we don't infinitely loop around
	unsigned long startSlot = slot;

	//Search for the pointer
	while (allocDataMap[slot].ptr != ptr)
	{
		slot++;
		if (slot == allocatedSlots)
			slot = 0;
		if (slot == startSlot)
			throw exception("Retrieve requested for untracked pointer!");
	}

	//Return pointer
	return allocDataMap[slot];
}

void AllocMap::erase(void* ptr)
{
	//Get starting slot
	unsigned long slot = hasher(ptr) >> shift;
	//Store starting slot to make sure we don't infinitely loop
	unsigned long startSlot = slot;
	//Find the pointer
	while (allocDataMap[slot].ptr != ptr)
	{
		slot++;
		if (slot == allocatedSlots)
			slot = 0;
		if (slot == startSlot)
			throw exception("Erase data requested for untracked pointer!");
	}
	//Null the pointer once it's found, no need to change the rest of the data it'll get overwritten when the slot is reused.
	allocDataMap[slot].ptr = nullptr;
}

AllocMap::iterator AllocMap::begin()
{
	//Returns the pointer to the start of the list
	return allocDataMap;
}

AllocMap::iterator AllocMap::end()
{
	//Returns the pointer just past the end of the list
	return allocDataMap + allocatedSlots;
}
