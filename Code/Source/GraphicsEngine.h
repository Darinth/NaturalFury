// Name:
// GraphicsEngine.h
// Description:
// Header file for GraphicsEngine class.
// Notes:
// OS-Aware
// Sets up OpenGL context and provides functions for accessing portions of it in a controller fashion.

#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <Windows.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <memory>
#include <unordered_map>
#include <stack>
using namespace std;

#include <glload/gl_3_3.h>
#include <glload/gll.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vec3.h"
#include "TexturedTriangle.h"
#include "Lockable.h"
#include "GraphicsEngineStateVariable.h"

//Triangles used for a typical cube.
extern const TexturedTriangle cubeVertices[];
//Vertices used for drawing a cylinder
extern const TexturedVertex cylinderVertices[];

class ThreadSafeFstream;
class ShaderProgram;

class ResourceHandle;
class CubeModel;

enum class StateVariable
{
	DepthTest,
	DepthFunction,
	CullFace,
	CullFaceMode,
	FrontFace,
	ShaderProgram
};

class GraphicsEngine : public Lockable
{
private:
	friend class CubeModel;

	//DeviceContext
	HDC deviceContext;
	//RenderingContext
	HGLRC renderingContext;

	//Tracks current parameter states
	unordered_map < StateVariable, GraphicsEngineStateVariable > parameterStates;

	//Tracks values to be restored to when popEngineState() is called.
	stack<stack<pair<StateVariable, GraphicsEngineStateVariable>>> stateChangeStack;

	//Variables for setting up camera to clip matrix
	const float zNear; //Closest visible point
	const float zFar;  //Furthest visible point
	const float frustumScale;  //Increases/decreases zoom
	float screenRatio;   //Aspect ratio of viewport

	//Matrix to convert coordinate from model to world.
	glm::dmat4 modelToWorld;

	//Matrix to convert coordinates from world to camera. Multiplied with modelToWorld matrix and then convert to floats before being uploaded to openGL.
	glm::dmat4 worldToCamera;

	//VAO and Buffer for drawing plain old cylinder.
	//Used as placeholder for stuff there is no model for yet.
	GLuint cylinderVAO;
	GLuint cylinderBuffer;

	//Matrix for transformation from camera to clip space
	glm::mat4 cameraToClip;

	//Shader uniform for cube texture.
	GLuint cubeTextureUniform;
	//Sampler for getting data from cubeTexture
	GLuint cubeSampler;
	//cubeTexture openGL texture
	GLuint cubeTexture;
	GLuint colorCameraToClipMatrixUniform;
	GLuint colorModelToCameraMatrixUniform;
	//Number of textures loaded
	unsigned int cubeTextureCount;
	//Data for loaded cube textures
	vector<unsigned char> cubeTextureData;
	//Cube texture height and width
	unsigned int cubeTextureHeight;
	unsigned int cubeTextureWidth;

	//Variables to deal with matrix block.
	GLuint matrixBlockBuffer;
	GLint matrixBlockUniformOffsets[2];

	//openGL Texture to hold ASCII texture
	GLuint ASCIItexture;
	//VAO and buffer for ASCII text drawing
	GLuint ASCIIVAO;
	GLuint ASCIIBuffer;

	//Boolean value to indicate that the viewport needs updating
	bool viewportUpdateNeeded;
	//Current/new viewport height and width
	int viewportWidth;
	int viewportHeight;

	//makes a shader from a file
	GLuint makeShaderFromFile(GLenum shaderType, string filePath);

	//Perform viewport update, if needed
	void updateViewport();

	//Disable copy and move constructor.
	GraphicsEngine(const GraphicsEngine& target) = delete;
	GraphicsEngine(GraphicsEngine&& target) = delete;

	//Sets opengl state and tracks it so it can be later reverted.
	void internalSetEngineParameter(StateVariable param, GraphicsEngineStateVariable);
	void updateModelToCamera();
	void updateCameraToClip();

public:
	//Constructor. Takes a device context.
	GraphicsEngine(HDC deviceContext, shared_ptr<ResourceHandle> vertShaderBase, shared_ptr<ResourceHandle> fragShaderBase);
	~GraphicsEngine();

	//Claims the context. Unless otherwise stated, GraphicsContext functions may only be used on a claimed context.
	void claim();
	//Relinquish the claim on a context.
	void relinquish();

	//Getters for clip space variables
	float getZNear() const;
	float getZFar() const;
	float getFrustumScale() const;
	float getScreenRatio() const;

	//Is the context claimed by the current thread?
	bool isClaimed();

	//Sets the PoV for standard program drawing
	void setPOV(double x, double y, double z, double angDegZ, double angDegX);

	//Runs an openGL error check and prints errors to the appLogger.
	bool doErrorCheck();

	void prepProgram(ShaderProgram* program);

	//Draws a cylinder at the given coordinate with a given radius and height. Triangles is currently ignored, but will eventually specify the number of triangles to use to draw the cylinder sides.
	void drawCylinder(const glm::dmat4& modelToWorld, double radius, double height, int triangles);

	//Clears the screen
	void clearScreen();

	//Uploads the cube texture array to OpenGL
	void bindCubeTextureArray();
	//Adds a texture to the cube texture array
	unsigned int loadTextureToCubeArray(string fileName);

	//Writes text to the screen.
	void textToScreen(string text);

	//Sets the viewport.
	//This function can be called without claiming the context.
	//Viewport is not updated until the next draw command is issued by a thread that has claimed the GraphicsContext.
	void setViewport(int sizeX, int sizeY);

	//Used to save/restore the engine state.
	void pushEngineState();
	void popEngineState();

	//Sets opengl state and tracks it so it can be later reverted.
	void setEngineParameter(StateVariable param, GraphicsEngineStateVariable);

	void useProgram(ShaderProgram& shaderProgram);

	void setModelToWorld(glm::dmat4 modelToWorld);
	void setWorldToCamera(glm::dmat4 worldToCamera);
	void setCameraToClip(glm::mat4 cameraToClip);
	void swapBuffers();
};

#endif
