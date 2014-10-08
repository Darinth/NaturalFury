// Name:
// Globals.h
// Description:
// Defines variables that are available application-wide, such as the resource cache.
// All global variables must start with the word 'global'.
// Notes:
// OS-Unaware

#ifndef GLOBALS_H
#define GLOBALS_H

class ResourceCache;
extern ResourceCache *globalResourceCache;

class Logger;
extern Logger* globalLogger;

#endif