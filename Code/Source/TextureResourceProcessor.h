// Name:
// TextureResourceProcessor.h
// Description:
// Header file for TextureResourceProcessor class
// Pre-processes resources in to raw images for openGL
// Notes:
// OS-Unaware

#ifndef TEXTURE_RESOURCE_PROCESSOR_H
#define TEXTURE_RESOURCE_PROCESSOR_H

#include "IResourceProcessor.h"

class StringResourceProcessor : public IResourceProcessor
{
	virtual string getPattern();
	virtual bool checkRawFile(shared_ptr<ResourceHandle> resourceHandle);
	virtual bool processResource(shared_ptr<ResourceHandle> resourceHandle);
};

#endif
