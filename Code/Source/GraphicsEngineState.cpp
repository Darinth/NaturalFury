// Name:
// GraphicsEngineState.cpp
// Description:
// Implementation file for GraphicsEngineState class
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "GraphicsEngineState.h"
#include "GraphicsEngine.h"

GraphicsEngineState::GraphicsEngineState(GraphicsEngine* graphicsEngine) : graphicsEngine(graphicsEngine)
{

}

GraphicsEngineState::~GraphicsEngineState()
{

}

void GraphicsEngineState::activate()
{

}

void GraphicsEngineState::deactivate()
{
	graphicsEngine->popEngineState();
}
