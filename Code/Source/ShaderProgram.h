// Name:
// ShaderProgram.h
// Description:
// Header file for ShaderProgram class.
// Notes:
// OS-Unaware
// Sets up a shader for the graphics engine. 

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <memory>

class GraphicsEngine;
class ResourceHandle;

class ShaderProgram
{
private:
	unsigned int shaderProgram;
	unsigned int cameraToClipMatrixUniform;
	unsigned int modelToCameraMatrixUniform;
	unsigned int cubeTextureUniform;

public:
	ShaderProgram(GraphicsEngine* graphicsEngine, shared_ptr<ResourceHandle> vertexShader, shared_ptr<ResourceHandle> fragmentShader);
	~ShaderProgram();

	friend class GraphicsEngine;
};

#endif