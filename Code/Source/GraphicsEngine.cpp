// Name:
// GraphicsEngine.cpp
// Description:
// Implementation file for GraphicsEngine class.
// Notes:
// OS-Aware
// See GraphicsEngine.h for class usage details

#include <thread>
#include <fstream>
#include <sstream>

#include "GraphicsEngine.h"
#include "Logger.h"
#include "lodepng.h"
#include "ThreadSafeStream.h"

extern Logger* appLogger;

bool graphicsFunctionsLoaded = false;

thread::id nullThread = thread::id();

extern const TexturedTriangle cubeVertices[] =
{
	// Top
	{
		{ Vec3<float>(1.0f, 1.0f, 1.0f), 0, 255, 255 },
		{ Vec3<float>(1.0f, 1.0f, 0.0f), 0, 255, 0 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), 0, 0, 255 }
	},

	{
		{ Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), 0, 0, 0 },
		{ Vec3<float>(1.0f, 1.0f, 0.0f), 0, 255, 255 }
	},

	// Front
	{
		{ Vec3<float>(1.0f, 1.0f, 0.0f), 1, 255, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), 1, 255, 255 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), 1, 0, 0 }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), 1, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), 1, 0, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), 1, 255, 255 }
	},

	// Right
	{
		{ Vec3<float>(1.0f, 1.0f, 0.0f), 2, 0, 0 },
		{ Vec3<float>(1.0f, 1.0f, 1.0f), 2, 255, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), 2, 0, 255 }
	},

	{
		{ Vec3<float>(1.0f, 0.0f, 1.0f), 2, 255, 255 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), 2, 0, 255 },
		{ Vec3<float>(1.0f, 1.0f, 1.0f), 2, 255, 0 }
	},

	// Back
	{
		{ Vec3<float>(1.0f, 1.0f, 1.0f), 3, 0, 0 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), 3, 255, 0 },
		{ Vec3<float>(1.0f, 0.0f, 1.0f), 3, 0, 255 }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 1.0f), 3, 255, 255 },
		{ Vec3<float>(1.0f, 0.0f, 1.0f), 3, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), 3, 255, 0 }
	},

	// Left
	{
		{ Vec3<float>(0.0f, 1.0f, 1.0f), 4, 0, 0 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), 4, 255, 0 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), 4, 0, 255 }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), 4, 255, 255 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), 4, 0, 255 },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), 4, 255, 0 }
	},

	// Bottom
	{
		{ Vec3<float>(1.0f, 0.0f, 1.0f), 5, 255, 255 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), 5, 0, 255 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), 5, 255, 0 }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), 5, 0, 0 },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), 5, 255, 0 },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), 5, 0, 255 }
	}
};

extern const TexturedVertex cylinderVertices[] =
{
	// Vertices for exterior of cylinder
	{ Vec3<float>(0.0f, 0.0f, 1.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), 1, 128, 255 },
	{ Vec3<float>(-1.0f, 0.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, -0.70710678f), 1, 128, 255 },
	{ Vec3<float>(0.0f, 0.0f, -1.0f), 1, 0, 0 },
	{ Vec3<float>(0.70710678f, 1.0f, -0.70710678f), 1, 128, 255 },
	{ Vec3<float>(1.0f, 0.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.70710678f, 1.0f, 0.70710678f), 1, 128, 255 },
	{ Vec3<float>(0.0f, 0.0f, 1.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), 1, 128, 255 },

	// Vertices for top of cylinder
	{ Vec3<float>(0.0f, 1.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), 1, 128, 255 },
	{ Vec3<float>(0.70710678f, 1.0f, 0.70710678f), 1, 128, 255 },
	{ Vec3<float>(0.70710678f, 1.0f, -0.70710678f), 1, 128, 255 },
	{ Vec3<float>(-0.70710678f, 1.0f, -0.70710678f), 1, 128, 255 },
	{ Vec3<float>(-0.70710678f, 1.0f, 0.70710678f), 1, 128, 255 },

	// Vertices for bottom of cylinder
	{ Vec3<float>(0.0f, 0.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.0f, 0.0f, 1.0f), 1, 0, 0 },
	{ Vec3<float>(-1.0f, 0.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.0f, 0.0f, -1.0f), 1, 0, 0 },
	{ Vec3<float>(1.0f, 0.0f, 0.0f), 1, 0, 0 },
	{ Vec3<float>(0.0f, 0.0f, 1.0f), 1, 0, 0 },
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
		MessageBox(0, e.what(), "Error", 0);
	}

	delete fileText;

	//return shader number
	return shader;
}

GraphicsEngine::GraphicsEngine(HDC deviceContext) :zNear(0.05f), zFar(120.0f), frustumScale(1.0f), screenRatio(1.0), PoVX(0.0), PoVY(0.0), PoVZ(0.0), rotationX(0.0f), rotationY(0.0f)
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
	GLint maxTextureUnits;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

	stringstream temp;
	temp << "Max Texture Units: " << maxTextureUnits << endl;
	appLogger->eWriteLog(temp.str().c_str(), LogLevel::Info, { "Graphics" });

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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	doErrorCheck();
	//Enable Face Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	doErrorCheck();
	//Setup Cylinder VAO & Buffer
	glGenVertexArrays(1, &cylinderVAO);
	glBindVertexArray(cylinderVAO);
	glGenBuffers(1, &cylinderBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderVertices), cylinderVertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16, 0);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, 16, (void *)(offsetof(TexturedVertex, texture)));
	glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_TRUE, 16, (void *)(offsetof(TexturedVertex, textureX)));

	doErrorCheck();
	//Make the vertex and fragment shader
	GLuint vertShader = makeShaderFromFile(GL_VERTEX_SHADER, "vertShader.vert");
	GLuint fragShader = makeShaderFromFile(GL_FRAGMENT_SHADER, "fragShader.frag");

	//Link the vertex and fragment shader
	try
	{
		theProgram = glutil::LinkProgram(vertShader, fragShader);
	}
	catch (glutil::CompileLinkException e)
	{
		//NOTE: Should probably move this to an error log rather than a message box for release.
		MessageBox(0, e.what(), "Error", 0);
	}

	//Check the program linking status
	GLint didLink;
	GLint logLength;
	char* infoLog;
	glGetProgramiv(theProgram, GL_LINK_STATUS, &didLink);

	if (didLink == GL_TRUE)
	{
		//		MessageBox(0, "Linking Successful", "Error", 0);
	}
	else
	{
		//If the program didn't link, get errors information and display error message.
		//NOTE: Should probably move this to an error log rather than a message box for release.
		MessageBox(0, "Linking Error", "Error", 0);
		glGetProgramiv(theProgram, GL_INFO_LOG_LENGTH, &logLength);
		infoLog = new char[logLength + 10];
		glGetProgramInfoLog(theProgram, logLength + 10, 0, infoLog);
		MessageBox(0, infoLog, "Error", 0);
		delete[] infoLog;
	}

	doErrorCheck();

	//Load in line shaders
	GLuint vertShaderLines = makeShaderFromFile(GL_VERTEX_SHADER, "vertShaderLines.vert");
	GLuint fragShaderLines = makeShaderFromFile(GL_FRAGMENT_SHADER, "fragShaderLines.frag");

	//Link the vertex and fragment shader
	try
	{
		lineProgram = glutil::LinkProgram(vertShaderLines, fragShaderLines);
	}
	catch (glutil::CompileLinkException e)
	{
		//NOTE: Should probably move this to an error log rather than a message box for release.
		MessageBox(0, e.what(), "Error", 0);
	}

	//Check the program linking status
	glGetProgramiv(lineProgram, GL_LINK_STATUS, &didLink);

	if (didLink == GL_TRUE)
	{
		//		MessageBox(0, "Linking Successful", "Error", 0);
	}
	else
	{
		//If the program didn't link, get errors information and display error message.
		//NOTE: Should probably move this to an error log rather than a message box for release.
		MessageBox(0, "Linking Error", "Error", 0);
		glGetProgramiv(lineProgram, GL_INFO_LOG_LENGTH, &logLength);
		infoLog = new char[logLength + 10];
		glGetProgramInfoLog(lineProgram, logLength + 10, 0, infoLog);
		MessageBox(0, infoLog, "Error", 0);
		delete[] infoLog;
	}

	doErrorCheck();

	//Get the uniform locations for the texture shaders
	cameraToClipMatrixUniform = glGetUniformLocation(theProgram, "cameraToClipMatrix");
	modelToCameraMatrixUniform = glGetUniformLocation(theProgram, "modelToCameraMatrix");
	cubeTextureUniform = glGetUniformLocation(theProgram, "cubeTexture");

	//Get the uniform locations for the color shaders
	colorCameraToClipMatrixUniform = glGetUniformLocation(lineProgram, "cameraToClipMatrix");
	colorModelToCameraMatrixUniform = glGetUniformLocation(lineProgram, "modelToCameraMatrix");

	//initialize cube texture count
	cubeTextureCount = 0;

	doErrorCheck();
	//That using program
	glUseProgram(theProgram);
	//Upload camera to clip matrix
	glUniformMatrix4fv(cameraToClipMatrixUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
	glUniformMatrix4fv(colorCameraToClipMatrixUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
	//Set cube texture uniform
	glUniform1i(cubeTextureUniform, 0);
	//Set cubeSampler
	glGenSamplers(1, &cubeSampler);

	//Low anisotropic
	glSamplerParameteri(cubeSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(cubeSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameterf(cubeSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

	doErrorCheck();

	//Initialize everything for textToScreen function
	unsigned int asciiTextureWidth, asciiTextureHeight;
	vector<unsigned char> asciiTextureData;

	//Load the font data
	lodepng::decode(asciiTextureData, asciiTextureWidth, asciiTextureHeight, "ASCII.png");

	doErrorCheck();
	//Allocate a texture name
	glGenTextures(1, &ASCIItexture);
	doErrorCheck();

	//Bind the texture
	glBindTexture(GL_TEXTURE_2D_ARRAY, ASCIItexture);
	doErrorCheck();
	//Setup texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	doErrorCheck();
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	doErrorCheck();
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	doErrorCheck();
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	doErrorCheck();
	//Upload texture to openGL
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, asciiTextureWidth, asciiTextureHeight / 95, 95, 0, GL_RGBA, GL_UNSIGNED_BYTE, &asciiTextureData[0]);
	doErrorCheck();

	//Setup VAO and Buffer for ASCII texture
	glGenVertexArrays(1, &ASCIIVAO);
	glBindVertexArray(ASCIIVAO);

	doErrorCheck();
	glGenBuffers(1, &ASCIIBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ASCIIBuffer);
	glBufferData(GL_ARRAY_BUFFER, 1, 0, GL_DYNAMIC_DRAW);

	doErrorCheck();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16, 0);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, 16, (void *)(offsetof(TexturedVertex, texture)));
	glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_TRUE, 16, (void *)(offsetof(TexturedVertex, textureX)));

	doErrorCheck();

	//Get current screen dimensions
	int screenDims[2];
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, screenDims);
	//Calculate aspect ratio
	screenRatio = 1.0f*screenDims[1] / screenDims[0];
	//Adjust camera to clip matrix
	cameraToClip[0].x = frustumScale * screenRatio;
	//Upload camera to clip matrix
	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
	glUseProgram(lineProgram);
	glUniformMatrix4fv(colorCameraToClipMatrixUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
}

GraphicsEngine::~GraphicsEngine()
{
	//Log error if graphics engine deconstructed for some reason while still claimed, this shouldn't ever happen.
	if (isClaimed())
		appLogger->eWriteLog("GraphicsEngine deconstructed while still owned.", LogLevel::Warning, { "Graphics" });

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

void GraphicsEngine::setPOV(double x, double y, double z, float angDegZ, float angDegX)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	PoVX = x;
	PoVY = z;
	PoVZ = y;
	rotationY = angDegZ;
	rotationX = angDegX;
}

double GraphicsEngine::getPoVX() const
{
	lock_guard<recursive_mutex> lock(objectMutex);
	return PoVX;
}

double GraphicsEngine::getPoVY() const
{
	lock_guard<recursive_mutex> lock(objectMutex);
	return PoVY;
}

double GraphicsEngine::getPoVZ() const
{
	lock_guard<recursive_mutex> lock(objectMutex);
	return PoVZ;
}

float GraphicsEngine::getRotationY() const
{
	lock_guard<recursive_mutex> lock(objectMutex);
	return rotationY;
}

float GraphicsEngine::getRotationX() const
{
	lock_guard<recursive_mutex> lock(objectMutex);
	return rotationX;
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
		appLogger->eWriteLog("doErrorCheck: GL_INVALID_ENUM", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_INVALID_VALUE)
	{
		appLogger->eWriteLog("doErrorCheck: GL_INVALID_VALUE", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_INVALID_OPERATION)
	{
		appLogger->eWriteLog("doErrorCheck: GL_INVALID_OPERATION", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		appLogger->eWriteLog("doErrorCheck: GL_INVALID_FRAMEBUFFER_OPERATION", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else if (error == GL_OUT_OF_MEMORY)
	{
		appLogger->eWriteLog("doErrorCheck: GL_OUT_OF_MEMORY", LogLevel::Warning, { "Graphics" });
		return true;
	}
	else
	{
		stringstream temp;
		temp << "doErrorCheck: UNKNOWN GL ERROR STATE: " << error;
		appLogger->eWriteLog(temp.str(), LogLevel::Warning, { "Graphics" });
		return true;
	}
}

void GraphicsEngine::prepStandardProgramDraw(const float* matrix, unsigned int VAO)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to prepStandardProgramDraw while context not active");
	//Update viewport if it needs it
	updateViewport();

	//Use the program
	glUseProgram(theProgram);

	//Upload the provided model to camera matrix
	glUniformMatrix4fv(modelToCameraMatrixUniform, 1, GL_FALSE, matrix);

	//Bind the VAO
	glBindVertexArray(VAO);
	//Bind textures
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, cubeTexture);

	doErrorCheck();
}

void GraphicsEngine::prepColoredProgramDraw(const float* matrix, unsigned int VAO)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to prepColoredProgramDraw while context not active");
	//Update viewport if it needs it
	updateViewport();

	//Use the program
	glUseProgram(lineProgram);

	//Upload the provided model to camera matrix
	glUniformMatrix4fv(colorModelToCameraMatrixUniform, 1, GL_FALSE, matrix);

	//Bind the VAO
	glBindVertexArray(VAO);

	doErrorCheck();
}

void GraphicsEngine::drawCylinder(Vec3<double> baseCoord, double radius, double height, int triangles)
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
	glutil::MatrixStack myStack;
	myStack.RotateX((float)rotationX);
	myStack.RotateY((float)rotationY);
	myStack.Translate((float)(-PoVX + baseCoord.x), (float)(-PoVY + baseCoord.z), (float)(-PoVZ + baseCoord.y));
	myStack.Scale((float)radius, (float)height, (float)radius);

	//Use the program
	glUseProgram(theProgram);

	//Upload modelToCamera matrix
	glUniformMatrix4fv(modelToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(myStack.Top()));

	//bind cylinder VAO
	glBindVertexArray(cylinderVAO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, cubeTexture);

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

void GraphicsEngine::bindCubeTextureArray()
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to bindCubeTextureArray while context not active");
	//Generate texture
	glGenTextures(1, &cubeTexture);
	//Activate and bind texture
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, cubeTexture);

	//Setup texture
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Load data to texture array.
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, cubeTextureHeight, cubeTextureWidth, cubeTextureCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubeTextureData[0]);
	//Generate mipmaps now!!!
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	//Log the cube array size
	stringstream temp;
	temp << "Cube Array Size: " << cubeTextureData.size();
	appLogger->eWriteLog(temp.str(), LogLevel::Info, { "Graphics" });

	//Clear the data out of the cube array
	cubeTextureData.clear();
}

unsigned int GraphicsEngine::loadTextureToCubeArray(string fileName)
{
	lock_guard<recursive_mutex> lock(objectMutex);
	if (!isClaimed())
		throw exception("Request to loadTextureToCubeArray while context not active");
	//Holds data
	vector<unsigned char> data;
	unsigned int width, height;
	unsigned int oldSize;

	//load the the data into the texture
	lodepng::decode(data, width, height, fileName.c_str());

	//If this is the first texture, save the width and height
	if (cubeTextureCount == 0)
	{
		cubeTextureWidth = width;
		cubeTextureHeight = height;
	}

	//If width & height of new textures are incorrect, abort.
	if (width != cubeTextureWidth || height != cubeTextureHeight)
		return 0;

	//Save the current texture size
	oldSize = cubeTextureData.size();
	//Resize the cubeTextureData to hold the new data
	cubeTextureData.resize(cubeTextureData.size() + data.size());
	//Copy the data into the cubeTextureData array.
	copy(data.begin(), data.end(), cubeTextureData.begin() + oldSize);

	return cubeTextureCount++;
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

	//Use the program
	glUseProgram(theProgram);
	doErrorCheck();

	//Upload the matrix
	glUniformMatrix4fv(modelToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelToCamera));
	doErrorCheck();

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
	//Upload cameraToClip matrix
	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
	glUseProgram(lineProgram);
	glUniformMatrix4fv(colorCameraToClipMatrixUniform, 1, GL_FALSE, glm::value_ptr(cameraToClip));
	//Turn off viewport update needed
	viewportUpdateNeeded = false;
}
