// Name:
// ThreadSafeStream.cpp
// Description:
// Header & Implementation file for ThreadSafeStream class.
// Provides a wrapper around a stream for thread safety.
// Notes:
// OS-Unaware
// None of the traditional stream functions return a stream, this is intentional as chaining together calls like that is inherently not thread safe


#ifndef THREAD_SAFE_STREAM_H
#define THREAD_SAFE_STREAM_H

#include "lockable.h"

template <class S>
class ThreadSafeStream :  public Lockable
{
private:
	S stream;
public:
	ThreadSafeStream() : stream(){}
	ThreadSafeStream(const char * fileName, ios_base::openmode openMode) : stream(fileName, openMode){}
	//template <class ...Args>
	//ThreadSafeStream(Args& ...args) : stream(args...){}

	template <class T>
	void operator>>(T input){
		lock_guard<recursive_mutex> lock(objectMutex);

		stream << input;
	}
	template <class T>	
	void operator<<(T input){
		lock_guard<recursive_mutex> lock(objectMutex);

		stream << input;
	}


	void close()
	{
		stream.close();
	}

	void open (const char* filename)
	{
		stream.open(filename);
	}

	void open(const string filename)
	{
		stream.open(filename.c_str());
	}

	
	int peek();
	
	void operator>> (ios_base& (*pf)(ios_base&))
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream >> pf;
	}

	void operator>> (istream& (*pf)(istream&))
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream >> pf;
	}

	void operator>> (ios& (*pf)(ios&))
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream >> pf;
	}

	void operator<< (ios_base& (*pf)(ios_base&))
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream << pf;
	}

	void operator<< (ostream& (*pf)(ostream&))
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream << pf;
	}

	void operator<< (ios& (*pf)(ios&))
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream << pf;
	}

	void putback(char c)
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream.putback(c);
	}

	void ignore(streamsize n = 1, int delim = EOF)
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream.ignore(n, delim);
	}
	
	void write(const char* s, streamsize n)
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream.write(s, n);
	}

	void flush()
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream.flush();
	}
	
	void getline(char* s, streamsize n)
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream.getline(s, n);
	}

	void getline(char* s, streamsize n, char delim)
	{
		lock_guard<recursive_mutex> lock(objectMutex);

		stream.getline(s, n, delim);
	}
};

#endif
