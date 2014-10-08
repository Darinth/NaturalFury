// Name:
// MasterDirectoryResourceSource.cpp
// Description:
// Implementation file for MasterDirectoryResourceSource class
// Notes:
// OS-Aware
// Uses OS-Specific functions to iterate over directory contents

#include "CustomMemory.h"

#include "MasterDirectoryResourceSource.h"

#include <Windows.h>

#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <algorithm>
using namespace std;

#include "Globals.h"
#include "Logger.h"
#include "DirectoryResourceSource.h"
#include "ZipResourceSource.h"

//Just store directory internally
MasterDirectoryResourceSource::MasterDirectoryResourceSource(string directory) : directory(directory) {}

MasterDirectoryResourceSource::~MasterDirectoryResourceSource()
{
	//Delete ResourceSources that were created during open.
	for (unordered_set<IResourceSource*>::iterator it = sourceList.begin(); it != sourceList.end(); it++)
	{
		delete (*it);
	}
}

bool MasterDirectoryResourceSource::open()
{
	stringstream findPath;
	HANDLE searchHandle;
	WIN32_FIND_DATA findData;
	BOOL nextFileResult;

	//Build string for the windows find function.
	findPath << directory << "\\*";

	//Open find
	searchHandle = FindFirstFile(findPath.str().c_str(), &findData);
	nextFileResult = 1;
	//If we've got a valid search handle...
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		//Iterate through the files & directories found
		while (nextFileResult != 0)
		{
			//Ignore files that start with a period, these are hidden and/or system files.
			if (findData.cFileName[0] != '.')
			{
				//Build path to file
				stringstream filePath;
				filePath << directory << "\\" << findData.cFileName;
				//ResourceSource to be initialized
				IResourceSource* source = nullptr;
				//If we've found a directory, initialize a DirectoryResourceSource for it
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					source = new DirectoryResourceSource(filePath.str().c_str());
				}
				//Otherwise, check for a zip file
				else
				{
					//Convert filename to lower case for testing
					string temp = filePath.str().c_str();
					transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
					//If we've got a zip file, initialize a ZipResourceSource for it
					if (temp.substr(temp.length() - 4, 4) == ".zip")
						source = new ZipResourceSource(filePath.str());
				}
				//If a source was created...
				if (source)
				{
					//Open the source
					source->open();
					//Add the source to the sourceList so it can be deleted during destruction
					sourceList.insert(source);
					//Get a list of files in the source
					unordered_set<string> files = source->getResourceList();
					//Add all of the files from the source into our file list
					for (unordered_set<string>::iterator it = files.begin(); it != files.end(); it++)
					{
						fileList[(*it)] = source;
					}
				}
			}
			//Get the next file
			nextFileResult = FindNextFile(searchHandle, &findData);
		}
		//Finished iterating over files
		FindClose(searchHandle);
	}
	//Something went wrong attempting to search the files in the directory.
	else
	{
		globalLogger->eWriteLog("Invalid directory path " + directory, LogLevel::Warning, { "Resource" });
	}

	return true;
}

int MasterDirectoryResourceSource::getRawResourceSize(const string &resource) const
{
	//If the selected file exists...
	if (fileList.count(resource) == 1)
		//Forward the size request to the correct ResourceSource
		return fileList.at(resource)->getRawResourceSize(resource);
	//If the resource isn't found, return 0.
	return 0;
}

int MasterDirectoryResourceSource::getRawResource(const string &resource, char * buffer) const
{
	//If the select file exists...
	if (fileList.count(resource) == 1)
		//Forward the resource request to the correct ResourceSource
		return fileList.at(resource)->getRawResource(resource, buffer);
	//If the resource isn't found, return 0.
	return 0;
}

int MasterDirectoryResourceSource::getNumResources() const
{
	//Return resource count
	return fileList.size();
}

string MasterDirectoryResourceSource::getResourceName(int num) const
{
	//Iterate over resource list to the specified resource and return it's name
	unordered_map<string, IResourceSource*>::const_iterator resource = fileList.begin();
	for (int I = 0; I < num; I++)
		++resource;
	return resource->first;
}

unordered_set<string> MasterDirectoryResourceSource::getResourceList() const
{
	//Copy resource from fileList to an unordered_set and return it
	unordered_set<string> result;
	result.reserve(fileList.size());
	for (unordered_map<string, IResourceSource*>::const_iterator it = fileList.begin(); it != fileList.end(); it++)
		result.insert(it->first);
	return result;
}
