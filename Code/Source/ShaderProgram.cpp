// Name:
// ShaderProgram.cpp
// Description:
// Implementation file for ShaderProgram class
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "ShaderProgram.h"

#include <sstream>
#include <memory>
using namespace std;

#include <glload/gl_4_2.hpp>
#include <glload/gll.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>

#include "Globals.h"
#include "ResourceHandle.h"
#include "GraphicsEngine.h"
#include "Logger.h"

ShaderProgram::ShaderProgram(GraphicsEngine* graphicsEngine, shared_ptr<ResourceHandle> vertexShader, shared_ptr<ResourceHandle> fragmentShader) : graphicsEngine(graphicsEngine)
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to construct a ShaderProgram with unclaimed GraphicsEngine");

	//Convert the point and spot light counts to text
	string pointLightCountText = static_cast<stringstream&>(stringstream() << graphicsEngine->getPointLightCount()).str();
	string spotLightCountText = static_cast<stringstream&>(stringstream() << graphicsEngine->getSpotLightCount()).str();

	//Replace #PointLightCount# with the number of point lights and #SpotLightCount# with number of spot lights in vertex shader.
	string vertexShaderSource = string(vertexShader->resource);
	if (vertexShaderSource.find("#PointLightCount#") != string::npos)
		vertexShaderSource.replace(vertexShaderSource.find("#PointLightCount#"), sizeof("#PointLightCount#") - 1, pointLightCountText);
	if (vertexShaderSource.find("#SpotLightCount#") != string::npos)
		vertexShaderSource.replace(vertexShaderSource.find("#SpotLightCount#"), sizeof("#SpotLightCount#") - 1, spotLightCountText);

	//Replace #PointLightCount# with the number of point lights and #SpotLightCount# with number of spot lights in fragment shader.
	string fragmentShaderSource = string(fragmentShader->resource);
	if (fragmentShaderSource.find("#PointLightCount#") != string::npos)
		fragmentShaderSource.replace(fragmentShaderSource.find("#PointLightCount#"), sizeof("#PointLightCount#") - 1, pointLightCountText);
	if (fragmentShaderSource.find("#SpotLightCount#") != string::npos)
		fragmentShaderSource.replace(fragmentShaderSource.find("#SpotLightCount#"), sizeof("#SpotLightCount#") - 1, spotLightCountText);


	//Comile vertex shader
	try
	{
		vertShader = glutil::CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	}
	catch (glutil::CompileLinkException e)
	{
		globalLogger->eWriteLog(e.what(), LogLevel::Error, { "Graphics" });
	}

	//Compile fragment shader
	try
	{
		fragShader = glutil::CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	}
	catch (glutil::CompileLinkException e)
	{
		globalLogger->eWriteLog(e.what(), LogLevel::Error, { "Graphics" });
	}

	//Link the vertex and fragment shader
	try
	{
		shaderProgram = glutil::LinkProgram(vertShader, fragShader);
	}
	catch (glutil::CompileLinkException e)
	{
		globalLogger->eWriteLog(e.what(), LogLevel::Error, { "Graphics" });
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
		globalLogger->eWriteLog(infoLog, LogLevel::Error, { "Graphics" });
		delete[] infoLog;
	}
}

ShaderProgram::~ShaderProgram()
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to deconstruct a ShaderProgram with unclaimed GraphicsEngine");

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);
}