// Name:
// GraphicsEngineTexture.h
// Description:
// Header file for GraphicsEngineTexture class.
// Notes:
// OS-Unaware
// Used with shared_ptrs to track the continued usage of a texture so it can be removed from openGL when nolonger in use.

#ifndef GRAPHICS_ENGINE_TEXTURE_H
#define GRAPHICS_ENGINE_TEXTURE_H

#include <string>
using namespace std;

class GraphicsEngine;

class GraphicsEngineTexture
{
	friend class GraphicsEngine;
private:
	const string textureName;
	GraphicsEngine* graphicsEngine;
	GraphicsEngineTexture(GraphicsEngine* graphicsEngine, string textureName, unsigned int textureNum);
public:
	const unsigned int textureNum;
	~GraphicsEngineTexture();
};

#endif