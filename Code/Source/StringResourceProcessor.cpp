// Name:
// StringResourceProcessor.cpp
// Description:
// Implementation file for StringResourceProcessor class
// Notes:
// OS-Unaware
// The Window class uses the pimpl idiom to avoid polution of windows header files into the rest of the project.

#include "CustomMemory.h"

#include "StringResourceProcessor.h"

#include <regex>
#include <memory>
using namespace std;

#include "ResourceHandle.h"
#include "ResourceCache.h"

char* pattern(".+\\.((?:vert)|(?:frag)|(?:txt))");

string StringResourceProcessor::getPattern()
{
	return pattern;
}

bool StringResourceProcessor::checkRawFile(shared_ptr<ResourceHandle> resourceHandle)
{
	return regex_match(resourceHandle->name, regex(pattern));
	return true;
}

bool StringResourceProcessor::processResource(shared_ptr<ResourceHandle> resourceHandle)
{
	if (checkRawFile(resourceHandle))
	{
		char* temp = resourceHandle->resourceCache->allocate(resourceHandle->resourceSize + 1);

		memcpy(temp, resourceHandle->resource, resourceHandle->resourceSize);

		temp[resourceHandle->resourceSize] = '\0';

		delete resourceHandle->resource;
		resourceHandle->resourceCache->memoryReleased(resourceHandle->resourceSize);

		resourceHandle->resource = temp;
		resourceHandle->resourceSize++;
		return true;
	}
	else
		return false;
}
