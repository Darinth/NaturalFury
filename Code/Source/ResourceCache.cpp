#include "CustomMemory.h"

#include <mutex>

#include "ResourceCache.h"
#include "ResourceHandle.h"
#include "IResourceSource.h"
#include "IResourceProcessor.h"
#include "Logger.h"

extern Logger* appLogger;

ResourceCache::ResourceCache(unsigned int size, IResourceSource *resourceSource) : availableMemory(size), resourceSource(resourceSource), allocatedMemory(0)
{
}

ResourceCache::~ResourceCache()
{
}

shared_ptr<ResourceHandle> ResourceCache::load(const string &resourceName)
{
	int resourceSize;		//Size of the resource to be loaded
	char* resource;			//Buffer to hold the resource
	
	//Get size of resource
	resourceSize = resourceSource->getRawResourceSize(resourceName);
	//Allocate room for the resource
	resource = allocate(resourceSize);
	//Get the resource
	resourceSource->getRawResource(resourceName, resource);

	//Create handle to resource
	shared_ptr<ResourceHandle> resourceHandle(new ResourceHandle(resourceName, resource, resourceSize, this));

	//Perform resource processing if there is a resource processor to match the file.
	{
		bool resourceProcessed = false;
		list<shared_ptr<IResourceProcessor> >::iterator it = resourceProcessors.begin();
		//Iterate over the resource processors until they've all been checked or the resource was processed
		while (it != resourceProcessors.end() && resourceProcessed == false)
		{
			//If the resource processor is for this file, process the file.
			if ((*it)->checkRawFile(resourceHandle))
			{
				(*it)->processResource(resourceHandle);
				resourceProcessed = true;
			}
			//Move to the next resource processor
			it++;
		}
	}

	//Put the resource on the front of the freeQueue
	freeQueue.push_front(resourceHandle);

	//Store handle for future retrieval
	resourceHandleMap[resourceName] = resourceHandle;

	//Return handle
	return resourceHandle;
}

char *ResourceCache::allocate(unsigned int size)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Free resources until we have enough memory
	while ((availableMemory - allocatedMemory) < size && freeOneResource());

	//Add size to the allocated memory
	allocatedMemory += size;

	//Check if we're over our allocation limits, write a warning entry if we are
	if (allocatedMemory > availableMemory)
		appLogger->eWriteLog("ResourceCache over memory limit!", LogLevel::Warning, { "ResourceCache" });

	//Allocate memory and return pointer.
	return new char[size];
}

bool ResourceCache::freeOneResource()
{
	//Release our shared_ptr to the resource at the back of the freeQueue
	if (freeQueue.size() > 0)
	{
		freeQueue.pop_back();
		//Return true, we released a resource
		return true;
	}

	//Return false, we're out of resources to release
	return false;
}

shared_ptr<ResourceHandle> ResourceCache::gethandle(const string &resourceName)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	shared_ptr<ResourceHandle> result;

	//If we have a weak_ptr for the resource already...
	if (resourceHandleMap.count(resourceName))
	{
		//Try to get a shared_ptr for the resource
		result = resourceHandleMap[resourceName].lock();
		//If we fail to get the shared_ptr, remove the weak_ptr from the handle map.
		if (!result)
			resourceHandleMap.erase(resourceName);
		//Otherwise move the shared_ptr for this resource to the front of the freeQueue
		else
		{
			//Iterate over the list of shared_ptrs in the freeQueue to see if we can find a pointer to our resourceHandle
			list<shared_ptr<ResourceHandle>>::iterator it = freeQueue.begin();
			bool found = false;
			while (it != freeQueue.end() && !found)
			{
				if ((*it) == result)
					found = true;
				else
					it++;
			}
			//If we find it, erase it.
			if (found)
				freeQueue.erase(it);
			//Insert a handle at the front of the freeQueue
			freeQueue.push_front(result);
		}
	}

	//If we never got a result, the resource isn't loaded. Load the resource.
	if (!result)
		result = load(resourceName);

	//Return the resource.
	return result;
}

void ResourceCache::memoryReleased(unsigned int size)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Note the memory as having been released
	allocatedMemory -= size;
}

void ResourceCache::preLoad(const string &resourceName)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Load the resource, don't do anything with it.
	//Calling getHandle will only load the resource if it's not already loaded.
	gethandle(resourceName);
}

void ResourceCache::flush()
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Release all of our handles that are keeping resources alive.
	freeQueue.clear();
}

void ResourceCache::registerProcessor(shared_ptr<IResourceProcessor> resourceLoader)
{
	lock_guard<recursive_mutex> objectLock(objectMutex);

	//Register processor.
	resourceProcessors.push_back(resourceLoader);
}
