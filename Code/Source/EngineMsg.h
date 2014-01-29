// Name:
// EngineMsg.h
// Description:
// Header File for EngineMsg Class and simple derived classes.
// EngineMsg is a class used for GameViews and other classes to communicate through the GameEngine.
// Notes:
// OS-Unaware

#ifndef ENGINE_MSG_H
#define ENGINE_MSG_H

#include "KeyEnum.h"

enum class MsgTypeEnum
{
	Shutdown
};

class EngineMsg
{
public:
	MsgTypeEnum msgType;
	EngineMsg(MsgTypeEnum msgType);
};

class EngineMsgShutdown : public EngineMsg
{
public:
	EngineMsgShutdown();
};

//Template for implementing other msgs
/*
class EngineMsgKeyUp : public EngineMsg
{
public:
	KeyEnum key;
	EngineMsgKeyUp(KeyEnum key);
};
*/
#endif