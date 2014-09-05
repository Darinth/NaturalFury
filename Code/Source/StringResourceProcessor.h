// Name:
// StringResourceProcessor.h
// Description:
// Header file for StringResourceProcessor class
// Pre-processes resources in to c-strings (just adds a '/0 ' to the end)
// Notes:
// OS-Unaware

#include "IResourceProcessor.h"

class StringResourceProcessor : public IResourceProcessor
{
	virtual string getPattern();
	virtual bool checkRawFile(shared_ptr<ResourceHandle> resourceHandle);
	virtual bool processResource(shared_ptr<ResourceHandle> resourceHandle);
};
