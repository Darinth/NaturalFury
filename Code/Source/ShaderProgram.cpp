// Name:
// ShaderProgram.cpp
// Description:
// Implementation file for ShaderProgram class
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "ShaderProgram.h"

#include <memory>
using namespace std;

#include <glload/gl_3_3.h>
#include <glload/gll.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>

#include "ResourceHandle.h"
#include "GraphicsEngine.h"
#include "Logger.h"

extern Logger* appLogger;

ShaderProgram::ShaderProgram(GraphicsEngine* graphicsEngine, shared_ptr<ResourceHandle> vertexShader, shared_ptr<ResourceHandle> fragmentShader) : graphicsEngine(graphicsEngine)
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to construct a ShaderProgram with unclaimed GraphicsEngine");

	//Comile vertex shader
	GLuint vertShader;
	try
	{
		vertShader = glutil::CompileShader(GL_VERTEX_SHADER, vertexShader->resource);
	}
	catch (glutil::CompileLinkException e)
	{
		appLogger->eWriteLog(e.what(), LogLevel::Error, { "Graphics" });
	}

	//Compile fragment shader
	GLuint fragShader;
	try
	{
		fragShader = glutil::CompileShader(GL_FRAGMENT_SHADER, fragmentShader->resource);
	}
	catch (glutil::CompileLinkException e)
	{
		appLogger->eWriteLog(e.what(), LogLevel::Error, { "Graphics" });
	}

	//Link the vertex and fragment shader
	try
	{
		shaderProgram = glutil::LinkProgram(vertShader, fragShader);
	}
	catch (glutil::CompileLinkException e)
	{
		appLogger->eWriteLog(e.what(), LogLevel::Error, { "Graphics" });
	}

	//Check the program linking status
	GLint didLink;
	GLint logLength;
	char* infoLog;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &didLink);

	if (didLink == GL_TRUE)
	{
		//		MessageBox(0, "Linking Successful", "Error", 0);
	}
	else
	{
		//If the program didn't link, get errors information and display error message.
		//NOTE: Should probably move this to an error log rather than a message box for release.
		MessageBox(0, "Linking Error", "Error", 0);
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		infoLog = new char[logLength + 10];
		glGetProgramInfoLog(shaderProgram, logLength + 10, 0, infoLog);
		appLogger->eWriteLog(infoLog, LogLevel::Error, { "Graphics" });
		delete[] infoLog;
	}

	//Get the uniform locations for the texture shaders
	cameraToClipMatrixUniform = glGetUniformLocation(shaderProgram, "cameraToClipMatrix");
	modelToCameraMatrixUniform = glGetUniformLocation(shaderProgram, "modelToCameraMatrix");
	cubeTextureUniform = glGetUniformLocation(shaderProgram, "cubeTexture");

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

ShaderProgram::~ShaderProgram()
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to deconstruct a ShaderProgram with unclaimed GraphicsEngine");

	glDeleteProgram(shaderProgram);
}