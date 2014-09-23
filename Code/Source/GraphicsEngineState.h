// Name:
// GraphicsEngineState.h
// Description:
// Header file for GraphicsEngineState class.
// Notes:
// OS-Unaware
// Sets up the graphics engine for drawing.

#ifndef GRAPHICS_ENGINE_STATE_H
#define GRAPHICS_ENGINE_STATE_H

class GraphicsEngine;

class GraphicsEngineState
{
private:
	GraphicsEngine* graphicsEngine;
public:
	GraphicsEngineState(GraphicsEngine* graphicsEngine);
	virtual ~GraphicsEngineState();
	virtual void activate();
	virtual void deactivate();
};

#endif
