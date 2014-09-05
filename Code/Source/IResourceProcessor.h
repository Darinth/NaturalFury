// Name:
// IResourceProcessor.h
// Description:
// Header file for IResourceLoader class
// An IResourceProcessor simply contains methods for pre-processing resources
// Notes:
// OS-Unaware

#ifndef RESOURCE_PROCESSOR_H
#define RESOURCE_PROCESSOR_H

#include <string>
#include <memory>
using namespace std;

class ResourceHandle;

class IResourceProcessor
{
public:
	virtual string getPattern() = 0;
	virtual bool checkRawFile(shared_ptr<ResourceHandle> resource) = 0;
	virtual bool processResource(shared_ptr<ResourceHandle> resource) = 0;
};

#endif
