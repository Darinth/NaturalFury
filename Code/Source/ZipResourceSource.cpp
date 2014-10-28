// Name:
// ZipResourceSource.cpp
// Description:
// Implementation file for ZipResourceSource class
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "ZipResourceSource.h"

#include <zlib/unzip.h>
#include <tinyxml/tinyxml.h>
#include <unordered_set>
using namespace std;

#include "Globals.h"
#include "Logger.h"
#include "Format.h"

const unsigned int fileNameLength = 1024;

ZipResourceSource::ZipResourceSource(string fileName) : zipOpen(false), zipFileName(fileName), zipFile(nullptr) {}

ZipResourceSource::~ZipResourceSource()
{
	//If the zip file is open, close it.
	if (zipOpen)
	{
		unzClose(zipFile);
	}
}

bool ZipResourceSource::open()
{
	int result;
	unz_file_info fileInfo;
	char fileName[fileNameLength];
	unsigned long filePos;
	char* docTemp;
	TiXmlDocument manifestDoc;
	unordered_set<string> blackList;

	//Open the zip file
	zipFile = unzOpen(zipFileName.c_str());
	//Write a log and return false on error
	if (zipFile == nullptr)
	{
		globalLogger->eWriteLog(formatToString("Failed to open ZipFile \\{0}", zipFileName), LogLevel::Warning, { "Resource" });
		return false;
	}

	TiXmlElement *currentTag;

	//Locate manifest
	result = unzLocateFile(zipFile, "manifest.xml", 2);
	//If we failed to locate the manifest, write the log, close the zip, and return false.
	if (result != UNZ_OK)
	{
		globalLogger->eWriteLog(formatToString("ZipFile \\{0} contains no manifest.xml", zipFileName), LogLevel::Warning, { "Resource" });
		unzClose(zipFile);
		return false;
	}

	//Get the file info
	result = unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, fileNameLength, nullptr, 0, nullptr, 0);
	//If the failed to the the file info, write the log, close the zip, and return false.
	if (result != UNZ_OK)
	{
		globalLogger->eWriteLog(formatToString("Failed to get manifest.xml file info from \\{0}", zipFileName), LogLevel::Warning, { "Resource" });
		unzClose(zipFile);
		return false;
	}

	//Allocate space for the manifest.
	docTemp = new char[fileInfo.uncompressed_size+1];

	//Read the manifest from the zip file
	unzOpenCurrentFile(zipFile);
	unzReadCurrentFile(zipFile, docTemp, fileInfo.uncompressed_size);
	unzCloseCurrentFile(zipFile);

	//Null terminate the manifest file.
	docTemp[fileInfo.uncompressed_size] = 0;
	
	//Parse the manifest file
	manifestDoc.Parse(docTemp);

	//Release the space allocated for the manifest file
	delete docTemp;

	//Get the Blacklist element from the manifest
	currentTag = manifestDoc.RootElement()->FirstChildElement("Blacklist");
	//If there is a Blacklist...
	if (currentTag)
	{
		//Get the first File element from the Blacklist
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

	//Start iterating through files in zip file
	result = unzGoToFirstFile(zipFile);
	//If we have an error... write the log, close the zip, and return false
	if (result != UNZ_OK)
	{
		globalLogger->eWriteLog(formatToString("Failed to go to first file of \\{0}", zipFileName), LogLevel::Warning, { "Resource" });
		unzClose(zipFile);
		return false;
	}

	//Until we fail to get the next file...
	while (result == UNZ_OK)
	{
		//Get the file info and offset to the file
		result = unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, fileNameLength, nullptr, 0, nullptr, 0);
		filePos = unzGetOffset(zipFile);

		//If we have an error... write the log, close the zip, and return false
		if (result != UNZ_OK)
		{
			globalLogger->eWriteLog(formatToString("Failed to get file info from \\{0}", zipFileName), LogLevel::Warning, { "Resource" });
			unzClose(zipFile);
			return false;
		}

		//Unless the file is blacklisted, add it to the positionMap
		if (!(blackList.count(fileName) == 1))
		{
			positionMap[fileName] = filePos;
		}

		//Go to the next file
		result = unzGoToNextFile(zipFile);
	}

	//If the result doesn't say end of file, we've got an error.
	if (result != UNZ_END_OF_LIST_OF_FILE)
	{
		globalLogger->eWriteLog(formatToString("Error occured reading \\{0} before end of file", zipFileName), LogLevel::Warning, { "Resource" });
		unzClose(zipFile);
		return false;
	}

	//Zip file is open
	zipOpen = true;

	//Return true
	return true;
}

int ZipResourceSource::getRawResourceSize(const string &resource) const
{
	int result;
	unz_file_info fileInfo;
	char fileName[fileNameLength];

	//Can't get resources with closed zip file
	if (!zipOpen)
	{
		globalLogger->eWriteLog(formatToString("Attempt to get resource information from unopened zip file: \\{0}", zipFileName), LogLevel::Warning, { "Resource" });
		return 0;
	}

	//File not found
	if (positionMap.count(resource) == 0)
	{
		globalLogger->eWriteLog(formatToString("File \\{0} not found in \\{1}.", resource, zipFileName), LogLevel::Warning, { "Resource" });
		return 0;
	}

	//Set ourselves to the selected file
	unzSetOffset(zipFile, positionMap.at(resource));

	//Get file info
	result = unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, fileNameLength, nullptr, 0, nullptr, 0);

	//Error getting file info
	if (result != UNZ_OK)
	{
		globalLogger->eWriteLog(formatToString("Failed to retieve file info for \\{0} from \\{1}", resource, zipFileName), LogLevel::Warning, { "Resource" });
		return 0;
	}

	//Return size of file
	return fileInfo.uncompressed_size;
}

int ZipResourceSource::getRawResource(const string &resource, char * buffer) const
{
	int result;
	unz_file_info fileInfo;
	char fileName[fileNameLength];

	//Zip file not open
	if (!zipOpen)
	{
		globalLogger->eWriteLog(formatToString("Attempt to get resource from unopened zip file: \\{0}", zipFileName), LogLevel::Warning, { "Resource" });
		return 0;
	}

	//File not in zip file
	if (positionMap.count(resource) == 0)
	{
		globalLogger->eWriteLog(formatToString("File \\{0} not found in \\{1}.", resource, zipFileName), LogLevel::Warning, { "Resource" });
		return 0;
	}

	//Set position
	unzSetOffset(zipFile, positionMap.at(resource));

	//Get file info
	result = unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, fileNameLength, nullptr, 0, nullptr, 0);

	//Error: We failed to retrieve the info
	if (result != UNZ_OK)
	{
		globalLogger->eWriteLog(formatToString("Failed to retieve file info for \\{0} from \\{1}", resource, zipFileName), LogLevel::Warning, { "Resource" });
		return 0;
	}

	//Open and read the data from the file
	unzOpenCurrentFile(zipFile);
	unzReadCurrentFile(zipFile, buffer, fileInfo.uncompressed_size);
	unzCloseCurrentFile(zipFile);

	//retrun size of file
	return fileInfo.uncompressed_size;
}

int ZipResourceSource::getNumResources() const
{
	//Return number of files in the file list
	return positionMap.size();
}

string ZipResourceSource::getResourceName(int num) const
{
	//Iterate over resource list to the specified resoruce and return it's name
	unordered_map<string, unsigned long>::const_iterator resource = positionMap.begin();
	for (int I = 0; I < num; I++)
		++resource;
	return resource->first;
}

unordered_set<string> ZipResourceSource::getResourceList() const
{
	//Copy resource from fileList to an unordered_set and return it
	unordered_set<string> result;
	result.reserve(positionMap.size());
	for (unordered_map<string, unsigned long>::const_iterator it = positionMap.begin(); it != positionMap.end(); it++)
		result.insert(it->first);
	return result;
}
