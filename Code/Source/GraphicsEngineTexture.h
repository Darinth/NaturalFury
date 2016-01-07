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

class GraphicsEngineTexture final
{
	friend class GraphicsEngine;
private:
	//Name of the texture
	const string textureName;
	//Graphics engine it's loaded into
	GraphicsEngine* graphicsEngine;
	//GraphicsEngineTextures should never be copied
	GraphicsEngineTexture(GraphicsEngineTexture&) = delete;
	void operator=(GraphicsEngineTexture&) = delete;
	//A GraphicsEngineTexture should only ever be created by a GraphicsEngine
	GraphicsEngineTexture(GraphicsEngine* graphicsEngine, string textureName, unsigned char textureNum);
public:
	//Number of the texture used by TexturedVertex
	const unsigned char textureNum;
	~GraphicsEngineTexture();
};

#endif