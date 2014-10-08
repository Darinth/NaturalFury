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

enum class ResourceType : unsigned short
{
	Unknown = 0,
	Texture,
	ResourceTypeCount
};

class IResourceExtraData
{
public:
	virtual ResourceType getType() = 0;
};

class ExtraTextureData : IResourceExtraData
{
private:
public:
	const unsigned int width;
	const unsigned int height;

	ExtraTextureData(unsigned int width, unsigned int height) : width(width), height(height) {}
	virtual ResourceType getType() { return ResourceType::Texture; }
};

class ResourceHandle : Lockable
{
private:
public:
	ResourceHandle(string name, char* resource, unsigned int resourceSize, ResourceCache *resourceCache);
	virtual ~ResourceHandle();
	const string name;
	ResourceCache * const resourceCache;
	char* resource;
	unsigned int resourceSize;
};

#endif
