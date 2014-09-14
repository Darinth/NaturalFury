// Name:
// GraphicsEngineState.h
// Description:
// Header file for GraphicsEngineState class.
// Notes:
// OS-Unaware
// Sets up the graphics engine for drawing.

class GraphicsEngine;

class GraphicsEngineState
{
private:
	GraphicsEngine* graphicsEngine;
public:
	GraphicsEngineState(GraphicsEngine* graphicsEngine);
	virtual ~GraphicsEngineState();
	virtual void activate() = 0;
	virtual void deactivate() = 0;
};
