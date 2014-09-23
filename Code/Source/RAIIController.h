// Name:
// RAIIController.h
// Description:
// Header file for RAIIController class.
// Notes:
// OS-Unaware
// Simple class that, when constructed, calls a method of another class and then called a different member when deconstructed.

#ifndef RAII_CONTROLLER_H
#define RAII_CONTROLLER_H

template <class T, void(T::*constructorFunction)(), void(T::*destructorFunction)()>
class RAIIController
{
private:
	T* object;
public:
	RAIIController(T* object) : object(object)
	{
		(object->*constructorFunction)();
	}

	virtual ~RAIIController()
	{
		(object->*destructorFunction)();
	}
};

#endif
