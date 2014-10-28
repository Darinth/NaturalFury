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

//Enum to identify resource type for resources that use ExtraData
enum class ResourceType : unsigned short
{
	Unknown = 0,
	ResourceTypeCount
};

//Interface class, just here to require that all extra data types define themselves.
class IResourceExtraData
{
public:
	virtual ResourceType getType() = 0;
};

//Carries raw data for a resource. ResourceCaches use this class to encapsulate data, and them keep the ResourceHandle alive for controlled periods to reduce the need for hard drive access.
class ResourceHandle : Lockable
{
private:
public:
	ResourceHandle(string name, char* resource, unsigned int resourceSize, ResourceCache *resourceCache);
	virtual ~ResourceHandle();
	//Name of the resource
	const string name;
	//Resource cache that is tracking the resource size
	ResourceCache * const resourceCache;
	//The resource
	char* resource;
	//The size of the tracked resource.
	unsigned int resourceSize;
};

#endif
