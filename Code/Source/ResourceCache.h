// Name:
// ResourceCache.h
// Description:
// Header file for ResourceCache class
// A ResourceCache holds data read from the hard drive for a period of time so that it can be retrieved more quickly later
// Notes:
// OS-Unaware

#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include <map>
#include <list>
#include <memory>
#include <mutex>
using namespace std;

class ResourceHandle;
class IResourceSource;
class IResourceProcessor;

class ResourceCache
{
private:
	recursive_mutex objectMutex;
	list<shared_ptr<ResourceHandle> > freeQueue;
	map<string, weak_ptr<ResourceHandle> > resourceHandleMap;
	list<shared_ptr<IResourceProcessor> > resourceProcessors;
	IResourceSource *resourceSource;

	unsigned int allocatedMemory;
	unsigned int availableMemory;

	shared_ptr<ResourceHandle> load(const string &resource);
	bool makeRoom(unsigned int size);
	bool freeOneResource();

	friend class ResourceHandle;

public:
	//Consturctor
	//Takes size of cache and a IResourceSource which is used to obtain resources.
	ResourceCache(unsigned int size, IResourceSource *resourceSource);
	virtual ~ResourceCache();

	//Add a processor to pre-process resources before handles are returned.
	void registerProcessor(shared_ptr<IResourceProcessor> resourceProcessor);

	//Get a ResourceHandle to the requested resource, loading it if needed
	shared_ptr<ResourceHandle> gethandle(const string &resourceName);
	//Makes sure a particular resource is in the cache, but doesn't actually get the handle.
	void preLoad(const string &resourceName);
	//Gets rid of all of the shared_ptrs to handles
	void flush();

	char *allocate(unsigned int size);
	//Tells the ResourceCache that memory was released. Used by ResourceHandle to inform the ResourceCache when it is destroyed.
	void memoryReleased(unsigned int size);
};

#endif
