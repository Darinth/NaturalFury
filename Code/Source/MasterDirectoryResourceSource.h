// Name:
// MasterDirectoryResourceSource.h
// Description:
// Header file for MasterDirectoryResourceSource class
// MasterDirectoryResourceSource examines all of the zip files and subdirectories within a folder and attempts to initialize a DirectoryResourceSource or ZipResourceSource for each,
// it then merges the contents of each to give the game access to all of the files contained in all of the zips and directories.
// See IResourceSource.h for usage details.
// Notes:
// OS-Unaware

#ifndef MASTER_DIRECTORY_RESOURCE_SOURCE_H
#define MASTER_DIRECTORY_RESOURCE_SOURCE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
using namespace std;
#include "IResourceSource.h"

class MasterDirectoryResourceSource : public IResourceSource
{
private:
	string directory;
	unordered_map<string, IResourceSource*> fileList;
	unordered_set<IResourceSource*> sourceList;
public:
	MasterDirectoryResourceSource(string directory);
	virtual ~MasterDirectoryResourceSource();
	virtual bool open();
	virtual int getRawResourceSize(const string &resource) const;
	virtual int getRawResource(const string &resource, char * buffer) const;
	virtual int getNumResources() const;
	virtual string getResourceName(int num) const;
	virtual unordered_set<string> getResourceList() const;
};

#endif
