// Name:
// ShaderProgram.h
// Description:
// Header file for ShaderProgram class.
// Notes:
// OS-Unaware
// Sets up a shader for the graphics engine. 

#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <memory>

class GraphicsEngine;
class ResourceHandle;

class ShaderProgram
{
private:
	GraphicsEngine* graphicsEngine;
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