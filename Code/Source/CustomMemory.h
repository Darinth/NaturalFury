#ifndef CUSTOM_MEMORY_H
#define CUSTOM_MEMORY_H

#include <string>
using namespace std;

//New/Delete operators that track locations of memory allocation.
void* operator new(unsigned int size, const char* sourceFile, const char *funcName, unsigned int lineNum);
void* operator new[](unsigned int size, const char* sourceFile, const char *funcName, unsigned int lineNum);
void operator delete(void* ptr, const char* sourceFile, const char *funcName, unsigned int lineNum);
void operator delete[](void* ptr, const char* sourceFile, const char *funcName, unsigned int lineNum);

//Function that returns a string of all memory that wasn't unallocated for logging purposes.
string getAllocs();

//Redirects all usage of standard new operator to the tracking version.
#define new new(__FILE__, __FUNCTION__, __LINE__)

#endif
