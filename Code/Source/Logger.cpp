// Name:
// Logger.cpp
// Description:
// Implementation file for Logger class
// Notes:
// OS-Aware
// Uses OS-Specific functions for logging and displaying different kinds of errors.

#define _CRT_SECURE_NO_WARNINGS

#include "CustomMemory.h"

#include "Logger.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <set>
#include <tinyxml/tinyxml.h>
#include <Windows.h>

#include "ThreadSafeStream.h"

//Just a thread-safe version of localtime
void localtime_safe(time_t* t, tm& l)
{
	static mutex localtime_mutex;
	lock_guard<mutex> lock(localtime_mutex);
	tm* temp = localtime(t);
	l = *temp;
}

string escapeFileName(const string& fileName)
{
	//Used to construct directory path
	stringstream nameBuilder;

	//Set a '0' to be used for filling, used for escape sequences
	nameBuilder << setfill('0');

	//Iterate through the characters in ther file name
	for (unsigned int I = 0; I < fileName.size(); I++)
	{
		char temp = fileName[I];
		//List of allowed characters, if it is in this list just use the character
		if ((temp >= 32 && temp <= 33) ||
			(temp >= 35 && temp <= 41) ||
			(temp >= 43 && temp <= 46) ||
			(temp >= 48 && temp <= 57) ||
			(temp >= 61) ||
			(temp >= 64 && temp <= 91) ||
			(temp >= 93 && temp <= 95) ||
			(temp >= 97 && temp <= 123) ||
			(temp >= 125 && temp <= 126))
		{
			nameBuilder << temp;
		}
		else
			//Everything eles gets replcaed by an escape sequence
			nameBuilder << "%" << hex << setw(2) << (int)temp << dec << setw(0);
	}


	return nameBuilder.str();
}

void writeLog(const string &file, const string &message, LogLevel logLevel, const char *funcName, const char* sourceFile, unsigned int lineNum)
{
	//Create the debug message
	//Format is '[Level]YYYY-MM-DD HH:MM:SS sourceFile, functionname(line lineNumber)\nMessage'
	stringstream debugMessage;
	if (logLevel == LogLevel::Info)
		debugMessage << "[Info]";
	else if (logLevel == LogLevel::Warning)
		debugMessage << "[Warning]";
	else if (logLevel == LogLevel::Error)
		debugMessage << "[Error]";
	time_t t = time(0);   // get time now
	tm now;
	localtime_safe(&t, now);
	debugMessage << (now.tm_year + 1900) << '-' << (now.tm_mon + 1) << '-' << now.tm_mday << " ";
	debugMessage << (now.tm_hour) << ":" << now.tm_min << ":" << now.tm_sec << " ";
	debugMessage << sourceFile << ", " << funcName << "(line " << lineNum << ")" << endl;
	debugMessage << message << endl;

	fstream stream(file, ios_base::out);
	stream << debugMessage.str();
	stream.flush();

	//If we're above an info level log message, write to whatever console is available for debugging
	//ignore this when we're not running a debug build
#ifdef _DEBUG
	if (logLevel != LogLevel::Info)
	{
#ifdef _WINDOWS
		OutputDebugString(debugMessage.str().c_str());
#endif
	}
#endif

	//If this is an error, output to the error log and display a dialog box with the message
	//Don't worry about the dialog box if we're not in debug mode.
	if (logLevel == LogLevel::Error)
	{
#ifdef _DEBUG
#ifdef _WINDOWS
		int result = MessageBox(NULL, debugMessage.str().c_str(), "ERROR", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON3);
		if (result == IDABORT)
		{
			DebugBreak();
		}
		else if (result == IDIGNORE)
		{
			//Setup ignore code here so that the same error will be ignored in the future.
		}
#endif
#endif
	}
}

Logger::Logger(string initializationFile, string generalLog) : generalStream(generalLog.c_str(), ios_base::out), warningStream("Warning.log", ios_base::out), errorStream("Error.log", ios_base::out)
{
	//Load logger settings from XML document
	TiXmlDocument doc(initializationFile.c_str());
	doc.LoadFile();

	//Get first 'Tag' element
	TiXmlElement *currentTag = doc.RootElement()->FirstChildElement("Tag");
	//While we have a valid tag element...
	while (currentTag != nullptr)
	{
		//Get the tag name, filename, and whether the tag is enabled
		TagDefinition tagDefinition;
		string name;
		name = currentTag->Attribute("name");
		tagDefinition.file = currentTag->Attribute("file");
		currentTag->QueryBoolAttribute("enabled", &tagDefinition.enabled);
		//If we have a name and a filename, store the tag in the tag definitions.
		if (name.length() > 0 && tagDefinition.file.length() > 0)
		{
			tagDefinitions[name] = tagDefinition;
		}
		//Otherwise, we had a problem. Log an error!
		else
		{
			stringstream logStream;
			logStream << "Error in tag definition: \"" << name << "\"" << endl;
			writeLog(logStream.str(), LogLevel::Warning, {}, __FUNCTION__, __FILE__, __LINE__);
		}
		//Move to the next Tag
		currentTag = currentTag->NextSiblingElement("Tag");
	}
}

Logger::~Logger()
{
	for (unordered_map<string, ThreadSafeStream<fstream>*>::iterator it = logStreams.begin(); it != logStreams.end(); it++)
	{
		delete (it->second);
	}
}

void Logger::writeLog(const string &message, LogLevel logLevel, const initializer_list<string> &tags, const char *funcName, const char* sourceFile, unsigned int lineNum)
{
	//Create the debug message
	//Format is '[Level]YYYY-MM-DD HH:MM:SS sourceFile, functionname(line lineNumber)\nMessage'
	stringstream debugMessage;
	if (logLevel == LogLevel::Info)
		debugMessage << "[Info]";
	else if (logLevel == LogLevel::Warning)
		debugMessage << "[Warning]";
	else if (logLevel == LogLevel::Error)
		debugMessage << "[Error]";
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	debugMessage << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << " ";
	debugMessage << (now->tm_hour) << ":" << now->tm_min << ":" << now->tm_sec << " ";
	debugMessage << sourceFile << ", " << funcName << "(line " << lineNum << ")" << endl;
	debugMessage << message << endl;

	//If there are tags...
	if (tags.size() > 0)
	{
		set<ThreadSafeStream<fstream>*> outputStreams;
		//Cycle through the tags...
		for (initializer_list<string>::iterator it = tags.begin(); it != tags.end(); it++)
		{
			//If the tag is valid
			if (tagDefinitions.count(*it) > 0)
			{
				TagDefinition &tagDefinition = tagDefinitions[*it];
				//And the tag is enabled
				if (tagDefinition.enabled)
				{
					//If the stream isn't opened, open it
					if (logStreams.count(tagDefinition.file) == 0)
					{
						lock_guard<recursive_mutex> objectLock(objectMutex);
						logStreams[tagDefinition.file] = new ThreadSafeStream<fstream>(escapeFileName(tagDefinition.file).c_str(), ios_base::out);
					}
					//Add the stream to the list of streams we want to write to
					outputStreams.insert(logStreams[tagDefinition.file]);
				}
			}
		}
		//Write to the steams, assuming there are any.
		if (outputStreams.size() > 0)
		{
			for (set<ThreadSafeStream<fstream>*>::iterator it = outputStreams.begin(); it != outputStreams.end(); it++)
			{
				**it << debugMessage.str();
			}
		}
	}
	//If there aren't any tags, output to the general log
	else
	{
		generalStream << debugMessage.str();
		generalStream.flush();
	}
	
	//If we're above an info level log message, write to whatever console is available for debugging
	//ignore this when we're not running a debug build
#ifdef _DEBUG
	if (logLevel != LogLevel::Info)
	{
#ifdef _WINDOWS
		OutputDebugString(debugMessage.str().c_str());
#endif
	}
#endif

	//If this is a warning, output to the warning log
	if (logLevel == LogLevel::Warning)
	{
		warningStream << debugMessage.str();
	}

	//If this is an error, output to the error log and display a dialog box with the message
	//Don't worry about the dialog box if we're not in debug mode.
	if (logLevel == LogLevel::Error)
	{
		errorStream << debugMessage.str();
#ifdef _DEBUG
#ifdef _WINDOWS
		int result = MessageBox(NULL, debugMessage.str().c_str(), "ERROR", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON3);
		if (result == IDABORT)
		{
			DebugBreak();
		}
		else if (result == IDIGNORE)
		{
			//Setup ignore code here so that the same error will be ignored in the future.
		}
#endif
#endif
	}
}

void Logger::flush()
{
	for (unordered_map<string, ThreadSafeStream<fstream>*>::iterator it = logStreams.begin(); it != logStreams.end(); it++)
	{
		it->second->flush();
	}

}
