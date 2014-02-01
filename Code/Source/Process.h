// Name:
// Process.h
// Description:
// Header File for Process Class.
// Processes form the base that is used to get the GameEngine to process code.
// Process is an abstract class, concrete classes should implement trigger to perform it's actions once it is ready. 
// Notes:
// OS-Unaware

#ifndef PROCESS_H
#define PROCESS_H

#include <memory>

class Process
{
public:
	unsigned int triggerTick;
	//Performs the actions for the process.
	//Returns: shared_ptr to process to be added to the queue. Use a shared_ptr to itself to re-add the process to the queue.
	virtual std::shared_ptr<Process> trigger() = 0;
public:
	Process(unsigned int triggerTick);
};

#endif