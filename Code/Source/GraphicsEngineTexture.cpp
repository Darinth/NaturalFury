// Name:
// GraphicsEngineTexture.cpp
// Description:
// Implementation file for GraphicsEngineTexture class.
// Notes:
// OS-Unaware
// See GraphicsEngineTexture.h for class usage details

#include "CustomMemory.h"

#include "GraphicsEngineTexture.h"

#include "GraphicsEngine.h"

//Store all of the parameters for destructor
GraphicsEngineTexture::GraphicsEngineTexture(GraphicsEngine* graphicsEngine, string textureName, unsigned char textureNum) : graphicsEngine(graphicsEngine), textureName(textureName), textureNum(textureNum) {}

GraphicsEngineTexture::~GraphicsEngineTexture()
{
	//Tell the graphicsEngine to free the resources for this texture.
	graphicsEngine->freeTextureFromArray(textureName, textureNum);
}