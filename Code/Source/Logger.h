// Name:
// Logger.h
// Description:
// Header file for Logger class
// Contains methods for writing information to logs.
// Notes:
// OS-Unaware

#ifndef LOG_H
#define LOG_H

#include <initializer_list>
#include <string>
#include <fstream>
#include <unordered_map>
using namespace std;

#include "ThreadSafeStream.h"
#include "Lockable.h"

struct TagDefinition
{
	string file;
	bool enabled;
};

class Logger : Lockable
{
private:
	unordered_map<string, TagDefinition> tagDefinitions;
	unordered_map<string, ThreadSafeStream<fstream>*> logStreams;
public:
	enum class LogLevel
	{
		Error = 0,
		Warning = 1,
		Info = 2
	};

	//General stream for untagged writes
	ThreadSafeStream<fstream> generalStream;
	//Streams to write warnings and errors to, in addition to whatever streams they'd normally be written to.
	ThreadSafeStream<fstream> warningStream;
	ThreadSafeStream<fstream> errorStream;

public:
	Logger(string initializationFile, string generalLog);

	//Writes a message to the log
	void writeLog(const string &message, LogLevel logLevel, const initializer_list<string> &tags, const char *funcName, const char* sourceFile, unsigned int lineNum);
	//Ghost function, no implementation provided. Doesn't actually work, is always redirected by macro.
	//Having it here allows autocomplete to work in visual studio however.
	void eWriteLog(const string &message, LogLevel logLevel, const initializer_list<string> &tags);
#define eWriteLog(message, logLevel, tags) writeLog(message, logLevel, tags, __FUNCTION__, __FILE__, __LINE__)
};

#endif
