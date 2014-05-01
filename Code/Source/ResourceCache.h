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
using namespace std;

class ResourceHandle;
class IResourceSource;
class IResourceLoader;

class ResourceCache
{
private:
	list<shared_ptr<ResourceHandle> > freeQueue;
	map<string, weak_ptr<ResourceHandle> > resourceHandleMap;
	list<shared_ptr<IResourceLoader> > resourceLoaders;
	IResourceSource *resourceSource;

	unsigned int allocatedMemory;
	unsigned int availableMemory;

	shared_ptr<ResourceHandle> load(const string &resource);
	bool makeRoom(unsigned int size);
	bool freeOneResource();

public:
	ResourceCache(unsigned int size, IResourceSource *resourceSource);
	virtual ~ResourceCache();
	
	void registerLoader(shared_ptr<IResourceLoader> resourceLoader);

	shared_ptr<ResourceHandle> gethandle(const string &resourceName);
	void preLoad(const string &resourceName);
	void flush();
	char *allocate(unsigned int size);
	void memoryReleased(unsigned int size);
};

#endif
