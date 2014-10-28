// Name:
// Globals.h
// Description:
// Defines variables that are available application-wide, such as the resource cache.
// All global variables must start with the word 'global'.
// Notes:
// OS-Unaware
// Make sure that these values are initialized first and in the order they're specified in this file.
// Multiple application components rely on their presense and DO NOT check to make sure they were properly initialized.
// Failure to initialize these variables in the right order will result in undefined behavior, probably crashing.

#ifndef GLOBALS_H
#define GLOBALS_H

//Logger used for events.
class Logger;
extern Logger* globalLogger;

//ResourceCache used for pulling stuff into the application
class ResourceCache;
extern ResourceCache *globalResourceCache;

#endif