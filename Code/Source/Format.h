// Name:
// Format.h
// Description:
// Header file for string formatting functions
// Notes:
// OS-Unaware
// Use formatToStream to send a formatting string to a stream.
// Use formatToString to return a formatted string.
// Declare custom IFormatters for specific classes to provide custom formatting of classes. By default, uses operator<< functionality to provide string conversion.
// Format strings by using a '\' character to initialize an escape, and then a {#} or {#,<formatstring>}.
// # is the zero-base argument number, <formatstring> is custom-defined on a per-type basis. No default types define any custom formatting.
// Providing the string "This is a \{0}" and the argument "test" would output the string "This is a test". Don't forget to escape the '\' character so it actually appears in the string.

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class IFormatter
{
public:
	virtual void format(ostream& stream, const string &format) = 0;
};

template<class T>
class DataFormatter : public IFormatter
{
private:
	const T &data;
public:
	DataFormatter(const T &data) : data(data){}

	virtual void format(ostream& stream, const string &format)
	{
		stream << data;
	}
};

template <>
class DataFormatter < string > : public IFormatter
{
private:
	const string &data;
public:
	DataFormatter(const string &data) : data(data) {}

	virtual void format(ostream& stream, const string &format)
	{
		stream << data;
	}
};

template<class... Args>
void formatToStream(ostream& stream, const string &format, Args... args)
{
	char tempString[5000];
	int curChar = 0;
	static const int argCount = sizeof...(Args);
	IFormatter *argFormatters[] = { new DataFormatter<Args>(args)... };
	bool escapeActive = false;

	stringstream formatStream(format);

	char currentChar;
	formatStream.get(tempString, 5000, '\\');
	stream << tempString;
	while (formatStream && !formatStream.eof())
	{
		formatStream.get(currentChar);

		if (currentChar != '\\')
			throw exception("Bad programming exception!");

		formatStream.get(currentChar);
		if (currentChar == '\\')
		{
			stream << '\\';
		}
		else if (currentChar == '{')
		{
			bool formatExtracted = false;
			int parameterNumber;
			char parameterFormat[50];
			//TODO: Make a specific exception class for this exception.
			if (!(formatStream >> parameterNumber))
				throw exception("Invalid format.");

			if (!formatStream.get(currentChar))
				throw exception("Invalid Format.");

			if (currentChar == ',')
				formatStream.get(parameterFormat, 50, '}');
			else if (currentChar == '}')
				parameterFormat[0] = '\0';
			else
				throw exception("Invalid Format.");

			argFormatters[parameterNumber]->format(stream, parameterFormat);
		}
		//else
		//throw exception("Bad programming exception");
		formatStream.get(tempString, 5000, '\\');
		stream << tempString;
	}

	for (int I = 0; I < argCount; I++)
	{
		delete argFormatters[I];
	}
}

template<class... Args>
string formatToString(const string &format, Args... args)
{
	stringstream temp;
	formatToStream(temp, format, args...);
	return temp.str();
}
