// Name:
// EngineMsg.cpp
// Description:
// Implementation file for EngineMsg class and simple derived classes.
// Notes:
// OS-Unaware

#include "EngineMsg.h"

EngineMsg::EngineMsg(MsgTypeEnum msgType) : msgType(msgType){}
EngineMsgShutdown::EngineMsgShutdown() : EngineMsg(MsgTypeEnum::Shutdown){}
//EngineMsgMouseMove::EngineMsgMouseMove(short x, short y) : EngineMsg(MsgTypeEnum::KeyPress), x(x), y(y){}
