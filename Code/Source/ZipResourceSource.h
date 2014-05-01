// Name:
// ZipResourceSource.h
// Description:
// Header file for ZipResourceSource class
// ZipResourceSource provides the files located within a zip file.
// See IResourceSource.h for usage details.
// Notes:
// OS-Unaware

#ifndef ZIP_RESOURCE_SOURCE_H
#define ZIP_RESOURCE_SOURCE_H

#include <string>
#include <unordered_map>
using namespace std;
#include "IResourceSource.h"

typedef void *unzFile;

class ZipResourceSource : public IResourceSource
{
private:
	unzFile zipFile;
	string zipFileName;
	bool zipOpen;
	unordered_map<string, unsigned long> positionMap;
public:
	ZipResourceSource(string fileName);
	virtual ~ZipResourceSource();
	virtual bool open();
	virtual int getRawResourceSize(const string &resource) const;
	virtual int getRawResource(const string &resource, char * buffer) const;
	virtual int getNumResources() const;
	virtual string getResourceName(int num) const;
	virtual unordered_set<string> getResourceList() const;
};

#endif
