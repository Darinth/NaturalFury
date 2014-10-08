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

GraphicsEngineTexture::GraphicsEngineTexture(GraphicsEngine* graphicsEngine, string textureName, unsigned int textureNum) : graphicsEngine(graphicsEngine), textureName(textureName), textureNum(textureNum) {}

GraphicsEngineTexture::~GraphicsEngineTexture()
{
	graphicsEngine->freeTextureFromArray(textureName, textureNum);
}