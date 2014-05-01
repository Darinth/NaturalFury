// Name:
// IResourceSource.h
// Description:
// Header file for IResourceSource class
// IResourceSource is used to define a source of resources for the cache
// Notes:
// OS-Unaware

#ifndef I_RESOURCE_SOURCE_H
#define I_RESOURCE_SOURCE_H

#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

class IResourceSource
{
public:
	//Call Open once a ResourceSource is constructed.
	virtual bool open() = 0;
	//Call getRawResourceSize to get the size of a resource to allocate space for it
	virtual int getRawResourceSize(const string &resource) const = 0;
	//Use this to get a resource, make sure buffer has enough space with getRawResourceSize.
	virtual int getRawResource(const string &resource, char * buffer) const = 0;
	//Returns the number of resources in a ResourceSource
	virtual int getNumResources() const = 0;
	//Returns the name of a resource based on it's number. O(n) function, don't use unless absolutely needed. Function may be deleted.
	virtual string getResourceName(int num) const = 0;
	//Returns a list of the resources in the ResourceSource.
	virtual unordered_set<string> getResourceList() const = 0;
	virtual ~IResourceSource(){};
};

#endif
