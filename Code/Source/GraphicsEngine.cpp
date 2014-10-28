// Name:
// GraphicsEngine.cpp
// Description:
// Implementation file for GraphicsEngine class.
// Notes:
// OS-Aware
// See GraphicsEngine.h for class usage details

#define _CRT_SECURE_NO_WARNINGS

#include "CustomMemory.h"

#include <thread>
#include <fstream>
#include <sstream>

#include "Globals.h"
#include "GraphicsEngine.h"
#include "Logger.h"
#include "lodepng.h"
#include "ThreadSafeStream.h"
#include "ShaderProgram.h"
#include "GraphicsEngineStateVariable.h"
#include "GraphicsEngineTexture.h"
#include "ResourceHandle.h"
#include "ResourceCache.h"
#include "Format.h"

bool graphicsFunctionsLoaded = false;

thread::id nullThread = thread::id();

const int pointLightMaxCount = 1000;

extern const TexturedTriangle cubeVertices[] =
{
	// Top
	{
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 255, 255 },
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 255, 0 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 255 }
	},
	{
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 0 },
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 255, 255 }
	},

	// Front
	{
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), 1, 255, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), 1, 255, 255 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), 1, 0, 0 }
	},
	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), 1, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), 1, 0, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), 1, 255, 255 }
	},

	// Right
	{
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f), 2, 0, 0 },
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(1.0f, 0.0f, 0.0f), 2, 255, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f), 2, 0, 255 }
	},
	{
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 0.0f, 0.0f), 2, 255, 255 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f), 2, 0, 255 },
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(1.0f, 0.0f, 0.0f), 2, 255, 0 }
	},

	// Back
	{
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 3, 0, 0 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 3, 255, 0 },
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 3, 0, 255 }
	},
	{
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 3, 255, 255 },
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 3, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 3, 255, 0 }
	},

	// Left
	{
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), 4, 0, 0 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), 4, 255, 0 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), 4, 0, 255 }
	},
	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), 4, 255, 255 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), 4, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), 4, 255, 0 }
	},

	// Bottom
	{
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 5, 255, 255 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 5, 0, 255 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 5, 255, 0 }
	},
	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 5, 0, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 5, 255, 0 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 5, 0, 255 }
	}
};

extern const TexturedVertex cylinderVertices[] =
{
	// Vertices for exterior of cylinder
	{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), Vec3<float>(-0.70710678f, 0.0f, 0.70710678f), 1, 128, 255 },
	{ Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, -0.70710678f), Vec3<float>(-0.70710678f, 0.0f, -0.70710678f), 1, 128, 255 },
	{ Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, 0.0f, -1.0f), 1, 0, 0 },
	{ Vec3<float>(0.70710678f, 1.0f, -0.70710678f), Vec3<float>(0.70710678f, 0.0f, -0.70710678f), 1, 128, 255 },
	{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.70710678f, 1.0f, 0.70710678f), Vec3<float>(0.70710678f, 0.0f, 0.70710678f), 1, 128, 255 },
	{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), Vec3<float>(-0.70710678f, 0.0f, 0.70710678f), 1, 128, 255 },

	// Vertices for top of cylinder
	{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), Vec3<float>(0.0f, 1.0f, 0.0f), 1, 128, 255 },
	{ Vec3<float>(0.70710678f, 1.0f, 0.70710678f), Vec3<float>(0.0f, 1.0f, 0.0f), 1, 128, 255 },
	{ Vec3<float>(0.70710678f, 1.0f, -0.70710678f), Vec3<float>(0.0f, 1.0f, 0.0f), 1, 128, 255 },
	{ Vec3<float>(-0.70710678f, 1.0f, -0.70710678f), Vec3<float>(0.0f, 1.0f, 0.0f), 1, 128, 255 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), Vec3<float>(0.0f, 1.0f, 0.0f), 1, 128, 255 },

	// Vertices for bottom of cylinder
	{ Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), 1, 0, 0 },
};

bool loadGraphicsFunctions()
{
	return (LoadFunctions() != LS_LOAD_FAILED);
}

GraphicsEngine::GraphicsEngine(HDC deviceContext) :zNear(0.05f), zFar(120.0f), frustumScale(1.0f), screenRatio(1.0), modelToWorld(1.0), worldToCamera(1.0), textureArray(0), textureArrayCount(0)
{
	lock_guard<recursive_mutex> lock(objectMutex);

	//Store device context
	this->deviceContext = deviceContext;

	//Create & store rendering context
	renderingContext = wglCreateContext(deviceContext);

	//Activate the context
	wglMakeCurrent(deviceContext, renderingContext);

	//Attempt to load the graphics functions, if it fails throw an exception
	if (!graphicsFunctionsLoaded)
	{
		if (!loadGraphicsFunctions())
			throw exception("Failed to load graphics functions... uh oh...");

		graphicsFunctionsLoaded = true;
	}

	//Check for other openGL errors
	doErrorCheck();

	//Setup clear color and depth
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	//Check for other openGL errors
	doErrorCheck();

	//Get texture units and write it to a log.
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents);

	globalLogger->eWriteLog(formatToString("Max Texture Units: \\{0}\nMax Array Texture Layers: \\{1}\nMax Vertex Uniform Components: \\{2}\nMax Fragment Uniform Components: \\{3}", maxTextureUnits, maxArrayTextureLayers, maxVertexUniformComponents, maxFragmentUniformComponents), LogLevel::Info, { "Graphics" });


	//Create camera to clip matrix. Complex mathy stuffs.
	cameraToClip = glm::mat4(1.0);
	cameraToClip[0].x = frustumScale;
	cameraToClip[1].y = frustumScale;
	cameraToClip[2].z = (zNear + zFar) / (zFar - zNear);
	cameraToClip[3].z = 2 * zNear * zFar / (zNear - zFar);
	cameraToClip[2].w = 1.0;
	cameraToClip[3].w = 0.0;

	doErrorCheck();
	//Enable depth testing
	internalSetEngineParameter(StateVariable::DepthTest, GraphicsEngineStateVariable(GLboolean(GL_TRUE)));
	internalSetEngineParameter(StateVariable::DepthFunction, GraphicsEngineStateVariable(GLint(GL_LEQUAL)));

	doErrorCheck();
	//Enable Face Culling
	internalSetEngineParameter(StateVariable::CullFace, GraphicsEngineStateVariable(GLboolean(GL_TRUE)));
	internalSetEngineParameter(StateVariable::CullFaceMode, GraphicsEngineStateVariable(GLint(GL_BACK)));
	internalSetEngineParameter(StateVariable::FrontFace, GraphicsEngineStateVariable(GLint(GL_CCW)));

	doErrorCheck();

	//Get current screen dimensions
	int screenDims[2];
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, screenDims);
	//Calculate aspect ratio
	screenRatio = 1.0f*screenDims[1] / screenDims[0];
	//Adjust camera to clip matrix
	cameraToClip[0].x = frustumScale * screenRatio;

	//Upload camera to clip matrix

	doErrorCheck();

	shared_ptr<ResourceHandle> vertShaderBase = globalResourceCache->gethandle("TextureShader.vert");
	shared_ptr<ResourceHandle> fragShaderBase = globalResourceCache->gethandle("TextureShader.frag");

	ShaderProgram tempProgram(this, vertShaderBase, fragShaderBase);

	//Setup matrix block
	GLuint matrixBlockIndex = glGetUniformBlockIndex(tempProgram.shaderProgram, "MatrixBlock");
	GLint matrixBlockSize;
	glGetActiveUniformBlockiv(tempProgram.shaderProgram, matrixBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &matrixBlockSize);
	glGenBuffers(1, &matrixBlockBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlockBuffer);
	glBufferData(GL_UNIFORM_BUFFER, matrixBlockSize, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, matrixBlockBuffer);

	glUseProgram(tempProgram.shaderProgram);
	const GLchar *matrixBlockUniformNames[] =
	{
		"MatrixBlock.modelToCameraMatrix",
		"MatrixBlock.cameraToClipMatrix"
	};

	GLuint matrixBlockUniformIndices[2];
	glGetUniformIndices(tempProgram.shaderProgram, 2, matrixBlockUniformNames, matrixBlockUniformIndices);

	glGetActiveUniformsiv(tempProgram.shaderProgram, 2, matrixBlockUniformIndices, GL_UNIFORM_OFFSET, matrixBlockUniformOffsets);

	doErrorCheck();

	//Setup light block
	GLuint lightBlockIndex = glGetUniformBlockIndex(tempProgram.shaderProgram, "LightBlock");
	GLint lightBlockSize;
	glGetActiveUniformBlockiv(tempProgram.shaderProgram, lightBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &lightBlockSize);
	glGenBuffers(1, &lightBlockBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
	glBufferData(GL_UNIFORM_BUFFER, lightBlockSize, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, lightBlockBuffer);

	glUseProgram(tempProgram.shaderProgram);
	const GLchar *lightBlockUniformNames[] =
	{
		"LightBlock.eyeDirection",
		"LightBlock.ambientLight",
		"LightBlock.sun.color",
		"LightBlock.sun.invDirection"
	};

	GLuint lightBlockUniformIndices[4];
	glGetUniformIndices(tempProgram.shaderProgram, 4, lightBlockUniformNames, lightBlockUniformIndices);

	glGetActiveUniformsiv(tempProgram.shaderProgram, 4, lightBlockUniformIndices, GL_UNIFORM_OFFSET, lightBlockUniformOffsets);

	glm::vec3 eyeDirection(0.0, 1.0, 0.0);

	glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, lightBlockUniformOffsets[0], 12, &(eyeDirection[0]));

	//Debug code, useful to pull a list of all of the uniforms in the program.
	int activeUniforms;
	glGetProgramiv(tempProgram.shaderProgram, GL_ACTIVE_UNIFORMS, &activeUniforms);

	stringstream uniforms;
	for (int I = 0; I < activeUniforms; I++)
	{
		int nameLength;
		char nameBuffer[50];
		glGetActiveUniformName(tempProgram.shaderProgram, I, 50, &nameLength, nameBuffer);
		uniforms << nameBuffer << endl;
	}

	//const int nameLen = strlen(uniformName) + 1;
	//const GLchar name[nameLen];
	//GLint uniformSize = 0;
	//GLenum uniformType = GL_NONE;
	//glGetActiveUniform(program, uniformIdx, nameLen, NULL,
	//	&uniformSize, &uniformType, name);

	//unsigned int pointLightCountUniform = glGetUniformLocation(shaderProgram, "numPointLights");
	//int pointLightCount;
	//glGetUniformiv(shaderProgram, pointLightCountUniform, &pointLightCount);

	//const GLchar **pointLightUniformNames = new char*[pointLightCount];
	//pointLightUniformNames[0] = new char[50];

	//Get uniform names for all of the different point lights
	{
		//ALL OF THIS POINT LIGHT CODE IS GOING TO BE REDONE
		//Holds names of the point lights
		char pointLightEnabledName[50];
		char pointLightColorName[50];
		char pointLightLocationName[50];
		//Workaround to deal with the fact that char** cannot be cast to const char**, copy the pointers after the data has been set.
		const GLchar *pointLightUniformNames[3];
		pointLightUniformNames[0] = pointLightEnabledName;
		pointLightUniformNames[1] = pointLightColorName;
		pointLightUniformNames[2] = pointLightLocationName;

		PointLightLocationData pointLightTemp[pointLightMaxCount];

		pointLightCount = 0;
		bool done = false;
		do
		{
			stringstream enabledName;
			enabledName << "LightBlock.pointLights[" << pointLightCount << "].enabled";
			strcpy(pointLightEnabledName, enabledName.str().c_str());

			stringstream colorName;
			colorName << "LightBlock.pointLights[" << pointLightCount << "].color";
			strcpy(pointLightColorName, colorName.str().c_str());

			stringstream locationName;
			locationName << "LightBlock.pointLights[" << pointLightCount << "].location";
			strcpy(pointLightLocationName, locationName.str().c_str());

			glGetUniformIndices(tempProgram.shaderProgram, 3, pointLightUniformNames, &pointLightTemp[pointLightCount].enabled);

			if (pointLightTemp[pointLightCount].enabled == -1)
			{
				done = true;
			}
			else
			{
				pointLightCount++;
			}
		}
		while (!done && pointLightCount < pointLightMaxCount);

		pointLightOffsets = new PointLightLocationData[pointLightCount];
	}
	

	doErrorCheck();

	//Update CameraToClip matrix
	updateCameraToClip();
	doErrorCheck();

	//Set modelToWorld & WorldToCamera matrices.
	setModelToWorld(glm::dmat4(1.0));
	setWorldToCamera(glm::dmat4(1.0));

	//Set ambient light levels
	setAmbientLight(glm::vec3(0.2, 0.2, 0.2));
	//Set sunlight
	setSunlight(glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.0, -1.0, 0.0));
}

GraphicsEngine::~GraphicsEngine()
{
	//Log error if graphics engine deconstructed for some reason while still claimed, this shouldn't ever happen.
	if (isClaimed())
		globalLogger->eWriteLog("GraphicsEngine deconstructed while still owned.", LogLevel::Warning, { "Graphics" });

	delete pointLightOffsets;

	//Delete rendering context
	wglDeleteContext(renderingContext);
}

void GraphicsEngine::claim()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	//Cannot claim an already claimed context.
	if (isClaimed())
		throw exception("Request to activate GraphicsEngine that is already active.");

	//Make the context current
	wglMakeCurrent(deviceContext, renderingContext);
}

void GraphicsEngine::relinquish()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	//Cannot deactivate unclaimed context
	if (!isClaimed())
		throw exception("Request to deactivate GraphicsEngine while not active or ");

	//Deactivate all contexts.
	wglMakeCurrent(deviceContext, nullptr);
}

float GraphicsEngine::getZNear() const
{
	return this->zNear;
}

float GraphicsEngine::getZFar() const
{
	return this->zFar;
}

float GraphicsEngine::getFrustumScale() const
{
	return this->frustumScale;
}

float GraphicsEngine::getScreenRatio() const
{
	return this->screenRatio;
}

bool GraphicsEngine::isClaimed()
{
	return wglGetCurrentContext() == renderingContext;
}

bool GraphicsEngine::doErrorCheck()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Cannot check for errors without having GraphicsEngine active");

	//Get error code
	GLenum error = glGetError();

	//If there's no error, return false
	if (error == GL_NO_ERROR)
	{
		return false;
	}
	//otherwise log the error and return true
	else if (error == GL_INVALID_ENUM)
	{
		globalLogger->eWriteLog("doErrorCheck: GL_INVALID_ENUM", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_INVALID_VALUE)
	{
		globalLogger->eWriteLog("doErrorCheck: GL_INVALID_VALUE", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_INVALID_OPERATION)
	{
		globalLogger->eWriteLog("doErrorCheck: GL_INVALID_OPERATION", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		globalLogger->eWriteLog("doErrorCheck: GL_INVALID_FRAMEBUFFER_OPERATION", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_OUT_OF_MEMORY)
	{
		globalLogger->eWriteLog("doErrorCheck: GL_OUT_OF_MEMORY", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else
	{
		globalLogger->eWriteLog(formatToString("doErrorCheck: UNKNOWN GL ERROR STATE: \\{0}", error), LogLevel::Warning, { "Graphics" });
		return true;
	}
}

void GraphicsEngine::drawCylinder(const glm::dmat4& modelToWorld, double radius, double height, int triangles)
{
	//ALL BROKEN CODE
	throw exception("drawCylinder not implemented, don't call this!");

	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to drawCylinder while context not active");
	//Update viewport if needed
	updateViewport();
	//Cannot draw cylinder with less than 3 triangles
	if (triangles < 3)
		return;

	//Setup modelToCamera matrix
	glm::mat4 modelToCamera(modelToWorld * worldToCamera);

	//bind cylinder VAO
	glBindVertexArray(cylinderVAO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

	doErrorCheck();

	//Draw cylinder
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
	glDrawArrays(GL_TRIANGLE_FAN, 10, 6);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 6);
	doErrorCheck();

}

void GraphicsEngine::clearScreen()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to clearScreen while context not active");
	//Update the viewport if needed
	updateViewport();
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsEngine::freeTextureFromArray(string name, unsigned int num)
{
	lock_guard<recursive_mutex> lock(objectMutex);

	//Remove weak_ptr to texture
	loadedTextures.erase(name);

	//Add texture index to list of available indexes.
	freeTextureIndices.push(num);
}

void GraphicsEngine::bindTextureArray()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to bindCubeTextureArray while context not active");
	//Delete old texture if we've done this before.
	if (textureArray)
		glDeleteTextures(1, &textureArray);
	//Generate texture
	glGenTextures(1, &textureArray);
	//Activate and bind texture
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

	//Setup texture
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Load data to texture array.
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, textureArrayHeight, textureArrayWidth, textureArrayCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureArrayData[0]);
	//Generate mipmaps now!!!
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	//Log the cube array size
	globalLogger->eWriteLog(formatToString("Cube Array Size: \\{0}", textureArrayData.size()), LogLevel::Info, { "Graphics" });
}

shared_ptr<GraphicsEngineTexture> GraphicsEngine::loadRawToTextureArray(string textureName, unsigned char* texture, unsigned int width, unsigned int height)
{
	lock_guard<recursive_mutex> lock(objectMutex);

	unsigned int oldSize;
	unsigned int dataSize = width * height * 4;

	//Just return a shared pointer if we've already loaded this texture.
	if (loadedTextures.count(textureName) > 0)
	{
		return loadedTextures[textureName].lock();
	}

	//If this is the first texture, save the width and height
	if (textureArrayCount == 0)
	{
		textureArrayWidth = width;
		textureArrayHeight = height;
	}

	//If width & height of new textures are incorrect, abort.
	if (width != textureArrayWidth || height != textureArrayHeight)
		return shared_ptr<GraphicsEngineTexture>();

	shared_ptr<GraphicsEngineTexture> returnTexture;
	//If we've got a free texture slot, put this one there.
	if (freeTextureIndices.size() > 0)
	{
		unsigned int selectIndex = freeTextureIndices.front();
		freeTextureIndices.pop();
		copy(texture, texture + dataSize, textureArrayData.begin() + dataSize*selectIndex);

		returnTexture = shared_ptr<GraphicsEngineTexture>(new GraphicsEngineTexture(this, textureName, selectIndex));
	}
	else
	{
		//Save the current texture size
		oldSize = textureArrayData.size();
		//Resize the cubeTextureData to hold the new data
		textureArrayData.resize(textureArrayData.size() + dataSize);
		//Copy the data into the cubeTextureData array.
		copy(texture, texture + dataSize, textureArrayData.begin() + oldSize);

		returnTexture = shared_ptr<GraphicsEngineTexture>(new GraphicsEngineTexture(this, textureName, textureArrayCount++));
	}

	loadedTextures[textureName] = returnTexture;

	return returnTexture;
}

shared_ptr<GraphicsEngineTexture> GraphicsEngine::loadPNGToTextureArray(string fileName)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to loadPNGToTextureArray while context not active");

	//Return shared_ptr to texture if it's already loaded.
	if (loadedTextures.count(fileName) > 0)
	{
		return loadedTextures[fileName].lock();
	}

	//Holds data
	vector<unsigned char> data;
	unsigned int width, height;

	//Get the handle to the data.
	shared_ptr<ResourceHandle> imageHandle = globalResourceCache->gethandle(fileName);

	//load the the data into the texture
	lodepng::decode(data, width, height, (unsigned char*)imageHandle->resource, imageHandle->resourceSize);

	//Load the texture into the texture array
	return loadRawToTextureArray(fileName, &data[0], width, height);
}

void GraphicsEngine::textToScreen(string text)
{
	//ALL BROKEN CODE
	throw exception("drawCylinder not implemented, don't call this!");

	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to textToScreen while context not active");
	updateViewport();
	doErrorCheck();

	//Enable blending so we can use transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Activate the font texture
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, ASCIItexture);
	doErrorCheck();

	//Holds vertices to draw text with
	vector<TexturedTriangle> vertices;

	//Reserve enough triangles for all of the text
	vertices.reserve(2 * text.length());
	//Temporarily holds a triangle
	TexturedTriangle myTriangle;

	//
	Vec3<float> myVector(0.0f, 0.0f, 0.0f);
	//For each character in the string
	for (unsigned int I = 0; I < text.size(); I++)
	{
		//Setup a vector to use to move the triangles over
		myVector.x = (float)I;
		//And for each of the two triangles we're going to need to use...
		for (int J = 0; J < 2; J++)
		{
			//Use the front triangles from the cube...
			myTriangle = cubeVertices[J + 2];
			///Move the triangles over...
			myTriangle += myVector;
			//Use a texture equal to the ASCII character -32
			myTriangle.point0.texture = text[I] - 32;
			myTriangle.point1.texture = text[I] - 32;
			myTriangle.point2.texture = text[I] - 32;
			//Push the triangles on to the vector
			vertices.push_back(myTriangle);
		}
	}
	//Disable depth testing
	glDisable(GL_DEPTH_TEST);
	doErrorCheck();

	//bind the VAO & buffer
	glBindVertexArray(ASCIIVAO);
	glBindBuffer(GL_ARRAY_BUFFER, ASCIIBuffer);
	//Upload triangle vertices
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(TexturedTriangle), &vertices[0], GL_DYNAMIC_DRAW);
	doErrorCheck();

	//Setup matrix specifically for drawing text
	glm::mat4 modelToCamera(1);
	modelToCamera[3].x = -15 / screenRatio;
	modelToCamera[3].y = -15;
	modelToCamera[3].z = 15;

	//Draw the text
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
	doErrorCheck();

	//Re-enable depth testing
	glEnable(GL_DEPTH_TEST);
	//Disable blending (probably not necessary)
	glDisable(GL_BLEND);
	doErrorCheck();
}

void GraphicsEngine::setViewport(int sizeX, int sizeY)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	//Determine that the viewport needs updating and store the values that it needs to be updated to
	viewportUpdateNeeded = true;
	viewportWidth = sizeX;
	viewportHeight = sizeY;
}

void GraphicsEngine::updateViewport()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to updateViewport while context not active");

	//If the viewport doesn't need updating, don't update it
	if (!viewportUpdateNeeded)
		return;

	//Update the viewport
	glViewport(0, 0, viewportWidth, viewportHeight);
	//Calculate new screen ratio
	screenRatio = 1.0f*viewportHeight / viewportWidth;
	//Calculate cameraToClip matrix
	cameraToClip[0].x = frustumScale * screenRatio;
	//Turn off viewport update needed
	viewportUpdateNeeded = false;
	//Update CameraToClip matrix
	updateCameraToClip();
}

void GraphicsEngine::pushEngineState()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to pushEngineState while context not active");

	//Create a new stack element to be popped off when popEngineState is called
	stateChangeStack.emplace();
}

void GraphicsEngine::popEngineState()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to popEngineState while context not active");

	//If the stack is empty, something has gone wrong.
	if (stateChangeStack.empty())
		throw exception("Attempt to pop engine state with empty stack");

	//get the top element off the stateChange stack.
	stack<pair<StateVariable, GraphicsEngineStateVariable>>& stateReversions = stateChangeStack.top();
	//Until we're out of stateReversions...
	while (!stateReversions.empty())
	{
		//Get a reversion, apply it, and remove it from the stack
		pair<StateVariable, GraphicsEngineStateVariable>& reversion = stateReversions.top();
		internalSetEngineParameter(reversion.first, reversion.second);
		stateReversions.pop();
	}
	//Pop the element we just reverted off the stateChangeStack.
	stateChangeStack.pop();
}

void GraphicsEngine::internalSetEngineParameter(StateVariable param, GraphicsEngineStateVariable value)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to setEngineParameter while context not active");

	//Get the type of the variable
	VariableType varType = value.getType();

	//Change the state of the variable requested
	if (varType == VariableType::Bool)
	{
		if (param == StateVariable::DepthTest)
		{
			if (value.getBool() == GL_TRUE)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
		else if (param == StateVariable::CullFace)
		{
			if (value.getBool() == GL_TRUE)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
		else
			throw exception("setEngineParameter called with invalid param/value combination");
	}
	else if (varType == VariableType::Int)
	{
		if (param == StateVariable::DepthFunction)
			glDepthFunc(value.getInt());
		else if (param == StateVariable::CullFaceMode)
			glCullFace(value.getInt());
		else if (param == StateVariable::FrontFace)
			glFrontFace(value.getInt());
		else if (param == StateVariable::ShaderProgram)
			glUseProgram(value.getInt());
		else
			throw exception("setEngineParameter called with invalid param/value combination");
	}
	else
		throw exception("setEngineParameter called with invalid value");

	//Mark the current state of the variable
	parameterStates[param] = value;
}

void GraphicsEngine::setEngineParameter(StateVariable param, GraphicsEngineStateVariable value)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to setEngineParameter while context not active");

	//Push the variable state change onto the stack, if this isn't our initial variable set, this is an actual change, and the stateChangeStack isn't empty
	if (parameterStates.count(param) == 1 && parameterStates[param] != value && !stateChangeStack.empty())
		stateChangeStack.top().push(pair<StateVariable, GraphicsEngineStateVariable>(param, parameterStates[param]));

	//Use internalSetEngineParameter to change the engine state if this is our first change, or it's different from the previous value
	if (parameterStates.count(param) == 0 || (parameterStates.count(param) == 1 && parameterStates[param] != value))
		internalSetEngineParameter(param, value);
}

void GraphicsEngine::useProgram(ShaderProgram& shaderProgram)
{
	//Use setEngineParameter to setup the program to be used.
	setEngineParameter(StateVariable::ShaderProgram, GraphicsEngineStateVariable((GLint)shaderProgram.shaderProgram));
}

void GraphicsEngine::setModelToWorld(glm::dmat4 modelToWorld)
{
	//update our modelToWorld matrix, and the push the update to openGL
	this->modelToWorld = modelToWorld;
	updateModelToCamera();
}

void GraphicsEngine::setWorldToCamera(glm::dmat4 worldToCamera)
{
	//Update our worldToCamera and push the update to openGL
	this->worldToCamera = worldToCamera;
	updateModelToCamera();
}

void GraphicsEngine::setCameraToClip(glm::mat4 cameraToClip)
{
	//Update our cameraToClip and push the update to OpenGL
	this->cameraToClip = cameraToClip;
	updateCameraToClip();
}

void GraphicsEngine::updateModelToCamera()
{
	//Calculate modelToCamera as combination of worldToCamera and modelToWorld
	modelToCamera = glm::mat4(worldToCamera * modelToWorld);

	//Push the new modelToCamera matrix to openGL UBO
	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, matrixBlockUniformOffsets[0], 64, &(modelToCamera[0][0]));
}

void GraphicsEngine::updateCameraToClip()
{
	//Push updated cameraToClip matrix to openGL UBO
	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, matrixBlockUniformOffsets[1], 64, &(this->cameraToClip[0][0]));
}

void GraphicsEngine::swapBuffers()
{
	//... swap the bloody buffers. :)
	SwapBuffers(deviceContext);
}

void GraphicsEngine::setAmbientLight(glm::vec3 ambientLight)
{
	//Update ambient light
	this->ambientLight = ambientLight;

	//Push ambient light update to openGL UBO
	glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, lightBlockUniformOffsets[1], 12, &this->ambientLight);
}

void GraphicsEngine::setSunlight(glm::vec3 color, glm::vec3 direction)
{
	//Update sunlight
	this->sunColor = color;
	this->sunDirection = glm::normalize(direction);

	//Push sunlight to openGL
	updateSunlight();
}

void GraphicsEngine::updateSunlight()
{
	//Get inverse direction of the sun, which is more useful from within the shaders
	glm::vec4 invDirection = -(modelToCamera * glm::vec4(this->sunDirection, 0.0));

	//Push sunColor and invDirection to openGL UBO
	glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, lightBlockUniformOffsets[2], 12, &(this->sunColor[0]));
	glBufferSubData(GL_UNIFORM_BUFFER, lightBlockUniformOffsets[3], 12, &(invDirection[0]));
}