// Name:
// DirectoryResourceSource.h
// Description:
// Header file for DirectoryResourceSource class
// DirectoryResourceSource provides the files located within a directory.
// See IResourceSource.h for usage details.
// Notes:
// OS-Unaware

#ifndef DIRECTORY_RESOURCE_SOURCE_H
#define DIRECTORY_RESOURCE_SOURCE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
using namespace std;
#include "IResourceSource.h"

class DirectoryResourceSource : public IResourceSource
{
private:
	string directory;
	unordered_map<string, unsigned long> fileList;
	void traverseFolder(unordered_set<string>& blackList, string folder);
public:
	DirectoryResourceSource(string directory);
	virtual ~DirectoryResourceSource();
	virtual bool open();
	virtual int getRawResourceSize(const string &resource) const;
	virtual int getRawResource(const string &resource, char * buffer) const;
	virtual int getNumResources() const;
	virtual string getResourceName(int num) const;
	virtual unordered_set<string> getResourceList() const;
};

#endif
