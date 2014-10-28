// Name:
// DirectoryResourceSource.cpp
// Description:
// Implementation file for DirectoryResourceSource class
// Notes:
// OS-Aware

#include "CustomMemory.h"

#include "DirectoryResourceSource.h"

#include <tinyxml/tinyxml.h>
#include <Windows.h>

#include <unordered_map>
#include <unordered_set>
#include <sstream>
using namespace std;

#include "Globals.h"
#include "Logger.h"
#include "Format.h"

const unsigned int fileNameLength = 1024;

//Just store directory internally
DirectoryResourceSource::DirectoryResourceSource(string directory) : directory(directory) {}

DirectoryResourceSource::~DirectoryResourceSource() {}

void DirectoryResourceSource::traverseFolder(unordered_set<string>& blackList, string folder)
{
	//Used to recursively traverse a directory structure
	stringstream findPath;
	HANDLE searchHandle;
	WIN32_FIND_DATA findData;
	BOOL nextFileResult;

	//Build string for the windows find function.
	findPath << directory;
	if (folder.length() > 0)
		findPath << "\\" << folder;
	findPath << "\\*";

	//Open find
	searchHandle = FindFirstFile(findPath.str().c_str(), &findData);
	nextFileResult = 1;

	//If we've got a valid search handle
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		//Iterate through the files & directories found
		while (nextFileResult != 0)
		{
			//Ignore files that start with a period, these are hidden and/or system files
			if (findData.cFileName[0] != '.')
			{
				//Build path to file
				stringstream filePath;
				if (folder.length() > 0)
					filePath << "\\" << folder;
				filePath << findData.cFileName;

				//If we've found a directory, recursively traverse it
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					traverseFolder(blackList, filePath.str());
				}
				//Otherwise, check if the file is blacklisted and add it to the list if it isn't
				else
				{
					if (blackList.count(filePath.str()) == 0)
					{
						fileList.emplace(filePath.str(), findData.nFileSizeLow);
					}
				}
			}
			//Get the next file
			nextFileResult = FindNextFile(searchHandle, &findData);
		}
		//Finished iterating over files
		FindClose(searchHandle);
	}
	//Something went wrong attempting to search the file in the directory.
	else
	{
		globalLogger->eWriteLog(formatToString("Invalid directory path \\{0}", folder), LogLevel::Warning, { "Resource" });
	}
}

bool DirectoryResourceSource::open()
{
	//Blacklist used to exclude files from Resource
	unordered_set<string> blackList;

	//Path to manifest file
	string manifestFile = directory + "\\manifest.xml";

	//Create and load document
	TiXmlDocument manifestDoc(manifestFile.c_str());
	TiXmlElement *currentTag;
	manifestDoc.LoadFile();

	//If we've got an error, write a log and return false.
	if (manifestDoc.Error())
	{
		globalLogger->eWriteLog(formatToString("Failed to open manifest.xml for \\{0} (\\{1})", directory, manifestDoc.ErrorDesc()), LogLevel::Warning, { "Resource" });
		return false;
	}

	//Retrieve BlackList element from XML
	currentTag = manifestDoc.RootElement()->FirstChildElement("Blacklist");
	//If there is a Blacklist...
	if (currentTag)
	{
		//Retrieve File element from BlackList
		currentTag = currentTag->FirstChildElement("File");
		//While we have a valid file element
		while (currentTag)
		{
			//Get the name of the file, add it to the blacklist, and move to the next File element
			string fileName;
			currentTag->QueryStringAttribute("name", &fileName);
			blackList.emplace(fileName);
			currentTag = currentTag->NextSiblingElement("File");
		}
	}

	//Begin traversing directory structure with the blackList
	traverseFolder(blackList, "");

	//All done
	return true;
}

int DirectoryResourceSource::getRawResourceSize(const string &resource) const
{
	//Return the size of the resource if it exists
	if (fileList.count(resource) == 1)
		return fileList.at(resource);
	//Return 0 if it doesn't
	return 0;
}

int DirectoryResourceSource::getRawResource(const string &resource, char * buffer) const
{
	//Get resource and return size of resource if it exists
	if (fileList.count(resource) == 1)
	{
		//Use fstream's read method to read all of the contents of the file into the buffer
		fstream file(directory + "//" + resource, ios::binary | ios::in);
		file.read(buffer, fileList.at(resource));
		//Return size of file
		return fileList.at(resource);
	}
	//Return 0 if file doesn't exist.
	return 0;
}

int DirectoryResourceSource::getNumResources() const
{
	//Return number of files in the file list
	return fileList.size();
}

string DirectoryResourceSource::getResourceName(int num) const
{
	//Iterate over resource list to the specified resource and return it's name
	unordered_map<string, unsigned long>::const_iterator resource = fileList.begin();
	for (int I = 0; I < num; I++)
		++resource;
	return resource->first;
}

unordered_set<string> DirectoryResourceSource::getResourceList() const
{
	//Copy resource from fileList to an unordered_set and return it
	unordered_set<string> result;
	result.reserve(fileList.size());
	for (unordered_map<string, unsigned long>::const_iterator it = fileList.begin(); it != fileList.end(); it++)
		result.insert(it->first);
	return result;
}
