// Name:
// GraphicsEngine.cpp
// Description:
// Implementation file for GraphicsEngine class.
// Notes:
// OS-Aware
// See GraphicsEngine.h for class usage details

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

bool graphicsFunctionsLoaded = false;

thread::id nullThread = thread::id();

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

GLuint GraphicsEngine::makeShaderFromFile(GLenum shaderType, string filePath)
{
	if (!isClaimed())
		throw exception("Attempt create shader for unclaimed GraphicsContext");

	lock_guard<recursive_mutex> lock(objectMutex);

	//Size of the file the shader is being made from
	unsigned int fileSize;
	//Text of shader
	char* fileText;
	//Shader number
	GLuint shader = 0;
	//Input filestream
	ifstream fileStream;

	//Enable exception for fail and bad operations
	fileStream.exceptions(ios::failbit | ios::badbit);

	//Open the file
	fileStream.open(filePath, ios::binary);
	//Move to end
	fileStream.seekg(0, ios::end);
	//Get location of end
	fileSize = (int)fileStream.tellg();
	//Move to begining
	fileStream.seekg(0);

	//Allocate enough storage for contents of file
	fileText = new char[fileSize + 100];

	//Read contents of file
	fileStream.read(fileText, fileSize);

	//Close file
	fileStream.close();

	//Add null terminator
	fileText[fileSize] = '\0';

	//Attempt to compile shader using gl utility library
	try
	{
		shader = glutil::CompileShader(shaderType, fileText);
	}
	catch (glutil::CompileLinkException e)
	{
		globalLogger->eWriteLog(e.what(), LogLevel::Error, { "Graphics" });
	}

	delete fileText;

	//return shader number
	return shader;
}

GraphicsEngine::GraphicsEngine(HDC deviceContext) :zNear(0.05f), zFar(120.0f), frustumScale(1.0f), screenRatio(1.0), modelToWorld(1.0), worldToCamera(1.0)
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

	//Get texture units and write it to a log.
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);

	stringstream temp;
	temp << "Max Texture Units: " << maxTextureUnits << endl;
	temp << "Max Array Texture Layers: " << maxArrayTextureLayers << endl;
	globalLogger->eWriteLog(temp.str().c_str(), LogLevel::Info, { "Graphics" });


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

	shared_ptr<ResourceHandle> vertShaderBase = globalResourceCache->gethandle("ColorShader.vert");
	shared_ptr<ResourceHandle> fragShaderBase = globalResourceCache->gethandle("ColorShader.frag");

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
		"LightBlock.ambientLight",
		"LightBlock.sun.color",
		"LightBlock.sun.invDirection"
	};

	GLuint lightBlockUniformIndices[4];
	glGetUniformIndices(tempProgram.shaderProgram, 3, lightBlockUniformNames, lightBlockUniformIndices);

	glGetActiveUniformsiv(tempProgram.shaderProgram, 3, lightBlockUniformIndices, GL_UNIFORM_OFFSET, lightBlockUniformOffsets);

	doErrorCheck();

	updateCameraToClip();
	doErrorCheck();

	setModelToWorld(glm::dmat4(1.0));
	setWorldToCamera(glm::dmat4(1.0));
	setAmbientLight(glm::vec3(0.2, 0.2, 1.0));
	setSunlight(glm::vec3(0.8, 0.8, 0.8), glm::vec3(-1.0, -1.0, -1.0));
}

GraphicsEngine::~GraphicsEngine()
{
	//Log error if graphics engine deconstructed for some reason while still claimed, this shouldn't ever happen.
	if (isClaimed())
		globalLogger->eWriteLog("GraphicsEngine deconstructed while still owned.", LogLevel::Warning, { "Graphics" });

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

void GraphicsEngine::setPOV(double x, double y, double z, double angDegZ, double angDegX)
{
	lock_guard<recursive_mutex> lock(objectMutex);

	worldToCamera = glm::dmat4();
	glm::gtc::matrix_transform::rotate(worldToCamera, angDegX, glm::dvec3(1, 0, 0));
	glm::gtc::matrix_transform::rotate(worldToCamera, angDegZ, glm::dvec3(0, 1, 0));
	glm::gtc::matrix_transform::translate(worldToCamera, glm::dvec3(x, y, z));
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
		stringstream temp;
		temp << "doErrorCheck: UNKNOWN GL ERROR STATE: " << error;
		globalLogger->eWriteLog(temp.str(), LogLevel::Warning, { "Graphics" });
		return true;
	}
}

void GraphicsEngine::prepProgram(ShaderProgram* program)
{

}

void GraphicsEngine::drawCylinder(const glm::dmat4& modelToWorld, double radius, double height, int triangles)
{
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsEngine::freeTextureFromArray(string name, unsigned int num)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to freeTextureArrayIndex while context not active");

	loadedTextures.erase(name);

	freeTextureIndices.push(num);
}

void GraphicsEngine::bindTextureArray()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to bindCubeTextureArray while context not active");
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
	stringstream temp;
	temp << "Cube Array Size: " << textureArrayData.size();
	globalLogger->eWriteLog(temp.str(), LogLevel::Info, { "Graphics" });
}

shared_ptr<GraphicsEngineTexture> GraphicsEngine::loadRawToTextureArray(string textureName, unsigned char* texture, unsigned int width, unsigned int height)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to loadRawToTextureArray while context not active");

	unsigned int oldSize;
	unsigned int dataSize = width * height * 4;

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
		return 0;

	//Save the current texture size
	oldSize = textureArrayData.size();
	//Resize the cubeTextureData to hold the new data
	textureArrayData.resize(textureArrayData.size() + dataSize);
	//Copy the data into the cubeTextureData array.
	copy(texture, texture + dataSize, textureArrayData.begin() + oldSize);

	shared_ptr<GraphicsEngineTexture> returnTexture(new GraphicsEngineTexture(this, textureName, textureArrayCount++));

	loadedTextures[textureName] = returnTexture;

	return loadedTextures[textureName].lock();
}

shared_ptr<GraphicsEngineTexture> GraphicsEngine::loadPNGToTextureArray(string fileName)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to loadPNGToTextureArray while context not active");

	if (loadedTextures.count(fileName) > 0)
	{
		return loadedTextures[fileName].lock();
	}

	//Holds data
	vector<unsigned char> data;
	unsigned int width, height;

	shared_ptr<ResourceHandle> imageHandle = globalResourceCache->gethandle(fileName);

	//load the the data into the texture
	lodepng::decode(data, width, height, (unsigned char*)imageHandle->resource, imageHandle->resourceSize);

	return loadRawToTextureArray(fileName, &data[0], width, height);
}

void GraphicsEngine::textToScreen(string text)
{
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
	updateCameraToClip();
}

void GraphicsEngine::pushEngineState()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to pushEngineState while context not active");

	stateChangeStack.emplace();
}

void GraphicsEngine::popEngineState()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to popEngineState while context not active");

	if (stateChangeStack.empty())
		throw exception("Attempt to pop engine state with empty stack");

	stack<pair<StateVariable, GraphicsEngineStateVariable>>& stateReversions = stateChangeStack.top();
	while (!stateReversions.empty())
	{
		pair<StateVariable, GraphicsEngineStateVariable>& reversion = stateReversions.top();
		internalSetEngineParameter(reversion.first, reversion.second);
		stateReversions.pop();
	}
	stateChangeStack.pop();
}

void GraphicsEngine::internalSetEngineParameter(StateVariable param, GraphicsEngineStateVariable value)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to setEngineParameter while context not active");

	VariableType varType = value.getType();

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

	parameterStates[param] = value;
}

void GraphicsEngine::setEngineParameter(StateVariable param, GraphicsEngineStateVariable value)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to setEngineParameter while context not active");

	if (parameterStates.count(param) == 1 && parameterStates[param] != value && !stateChangeStack.empty())
		stateChangeStack.top().push(pair<StateVariable, GraphicsEngineStateVariable>(param, parameterStates[param]));

	if (parameterStates.count(param) == 0 || (parameterStates.count(param) == 1 && parameterStates[param] != value))
		internalSetEngineParameter(param, value);
}

void GraphicsEngine::useProgram(ShaderProgram& shaderProgram)
{
	setEngineParameter(StateVariable::ShaderProgram, GraphicsEngineStateVariable((GLint)shaderProgram.shaderProgram));
}

void GraphicsEngine::setModelToWorld(glm::dmat4 modelToWorld)
{
	this->modelToWorld = modelToWorld;
	updateModelToCamera();
}

void GraphicsEngine::setWorldToCamera(glm::dmat4 worldToCamera)
{
	this->worldToCamera = worldToCamera;
	updateModelToCamera();
}

void GraphicsEngine::setCameraToClip(glm::mat4 cameraToClip)
{
	this->cameraToClip = cameraToClip;
	updateCameraToClip();
}

void GraphicsEngine::updateModelToCamera()
{
	glm::mat4 newModelToCamera = glm::mat4(worldToCamera * modelToWorld);

	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, matrixBlockUniformOffsets[0], 64, &(newModelToCamera[0][0]));
}

void GraphicsEngine::updateCameraToClip()
{
	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, matrixBlockUniformOffsets[1], 64, &(this->cameraToClip[0][0]));
}

void GraphicsEngine::swapBuffers()
{
	SwapBuffers(deviceContext);
}

void GraphicsEngine::setAmbientLight(glm::vec3 ambientLight)
{
	this->ambientLight = ambientLight;

	glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, lightBlockUniformOffsets[0], 12, &this->ambientLight);
}

void GraphicsEngine::setSunlight(glm::vec3 color, glm::vec3 direction)
{
	this->sunColor = color;
	this->sunDirection = glm::normalize(direction);

	glm::vec3 invDirection = -this->sunDirection;

	glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, lightBlockUniformOffsets[1], 12, &this->sunColor);
	glBufferSubData(GL_UNIFORM_BUFFER, lightBlockUniformOffsets[2], 12, &invDirection);
}