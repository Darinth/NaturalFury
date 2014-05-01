#include "CustomMemory.h"

#include "ResourceCache.h"
#include "ResourceHandle.h"
#include "IResourceSource.h"
#include "IResourceLoader.h"

ResourceCache::ResourceCache(unsigned int size, IResourceSource *resourceSource) : availableMemory(size), resourceSource(resourceSource)
{
	allocatedMemory = 0;
}

ResourceCache::~ResourceCache()
{
}

shared_ptr<ResourceHandle> ResourceCache::load(const string &resourceName)
{
	int resourceSize;
	char* resource;
	
	resourceSize = resourceSource->getRawResourceSize(resourceName);
	resource = allocate(resourceSize);
	resourceSource->getRawResource(resourceName, resource);

	shared_ptr<ResourceHandle> resourceHandle(new ResourceHandle(resourceName, resource, resourceSize, this));

	{
		bool resourceLoaded = false;
		list<shared_ptr<IResourceLoader> >::iterator it = resourceLoaders.begin();
		while(it != resourceLoaders.end() && resourceLoaded == false)
		{
			if ((*it)->checkRawFile(resourceHandle))
			{
				(*it)->loadResource(resourceHandle);
				resourceLoaded = true;
			}
			it++;
		}
	}

	freeQueue.push_front(resourceHandle);
	resourceHandleMap[resourceName] = resourceHandle;

	return resourceHandle;
}

char *ResourceCache::allocate(unsigned int size)
{
	while ((availableMemory - allocatedMemory) < size && freeOneResource());

	allocatedMemory += size;

	return new char[size];
}

bool ResourceCache::freeOneResource()
{
	if (freeQueue.size() > 0)
	{
		freeQueue.pop_back();
		return true;
	}

	return false;
}

shared_ptr<ResourceHandle> ResourceCache::gethandle(const string &resourceName)
{
	shared_ptr<ResourceHandle> result;
	if (resourceHandleMap.count(resourceName))
	{
		result = resourceHandleMap[resourceName].lock();
		if (!result)
			resourceHandleMap.erase(resourceName);
	}

	if (!result)
		result = load(resourceName);

	return result;
}

void ResourceCache::memoryReleased(unsigned int size)
{
	allocatedMemory -= size;
}

void ResourceCache::preLoad(const string &resourceName)
{
	load(resourceName);
}