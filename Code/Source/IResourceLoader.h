// Name:
// IResourceLoader.h
// Description:
// Header file for IResourceLoader class
// An IResourceLoader simply contains methods for pre-processing resources
// Notes:
// OS-Unaware

#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <string>
#include <memory>
using namespace std;

class ResourceHandle;

class IResourceLoader
{
public:
	virtual string getPattern() = 0;
	virtual bool checkRawFile(shared_ptr<ResourceHandle> resource) = 0;
	virtual shared_ptr<ResourceHandle> loadResource(shared_ptr<ResourceHandle> resource) = 0;
};

#endif