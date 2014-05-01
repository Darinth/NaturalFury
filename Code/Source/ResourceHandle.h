// Name:
// ResourceHandle.h
// Description:
// Header file for ResourceHandle class
// A ResourceHandle holds data loaded by a ResourceCache
// Notes:
// OS-Unaware

#ifndef RESOURCE_HANDLE_H
#define RESOURCE_HANDLE_H

#include <string>
using namespace std;

#include "Lockable.h"

class ResourceCache;

class ResourceHandle : Lockable
{
private:
	string name;
	char* resource;
	unsigned int resourceSize;
	ResourceCache *resourceCache;
public:
	ResourceHandle(string name, char* resource, unsigned int resourceSize, ResourceCache *resourceCache);
	virtual ~ResourceHandle();
	const char * const getResource() const
	{
		return resource;
	};
};

#endif
