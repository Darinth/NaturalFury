#include "CustomMemory.h"

#include "ResourceHandle.h"

#include <string>
using namespace std;

#include "ResourceCache.h"

ResourceHandle::ResourceHandle(string name, char* resource, unsigned int resourceSize, ResourceCache *resourceCache) : name(name), resource(resource), resourceSize(resourceSize), resourceCache(resourceCache)
{

}

ResourceHandle::~ResourceHandle()
{
	//Unallocate memory
	delete[] resource;
	//Tell the cache that the memory was released.
	resourceCache->memoryReleased(resourceSize);
}
