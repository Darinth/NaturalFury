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
#include <queue>
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
class GraphicsEngineTexture;

enum class StateVariable
{
	DepthTest,
	DepthFunction,
	CullFace,
	CullFaceMode,
	FrontFace,
	ShaderProgram
};

//Holds offsets into the openGL UBO for point lights
struct PointLightOffsetData
{
	int enabled;
	int color;
	int location;
	int attenuation;
};

//Holds actual point light data
struct PointLightData
{
	bool enabled;
	glm::vec3 color;
	glm::vec3 location;
	glm::vec3 attenuation;
};

//Holds offsets into the openGL UBO for spot lights
struct SpotLightOffsetData
{
	int enabled;
	int color;
	int location;
	int attenuation;
	int direction;
	int fullDot;
	int minDot;
};

//Holds actual spot light data
struct SpotLightData
{
	bool enabled;
	glm::vec3 color;
	glm::vec3 location;
	glm::vec3 attenuation;
	glm::vec3 direction;
	float fullDot;
	float minDot;
};

class GraphicsEngine : public Lockable
{
private:
	friend class CubeModel;
	friend class GraphicsEngineTexture;

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

	//Matrix to hold the composition of modelToWorld and worldToCamera.
	glm::mat4 modelToCamera;

	//VAO and Buffer for drawing plain old cylinder.
	//Used as placeholder for stuff there is no model for yet.
	GLuint cylinderVAO;
	GLuint cylinderBuffer;

	//Matrix for transformation from camera to clip space
	glm::mat4 cameraToClip;

	//Variables used when loading textures.
	GLint maxTextureUnits;
	GLint maxArrayTextureLayers;
	GLint maxVertexUniformComponents;
	GLint maxFragmentUniformComponents;

	//Shader uniform for cube texture.
	GLuint textureArrayUniform;
	//Sampler for getting data from cubeTexture
	GLuint arraySampler;
	//cubeTexture openGL texture
	GLuint textureArray;
	GLuint colorCameraToClipMatrixUniform;
	GLuint colorModelToCameraMatrixUniform;
	//Number of textures loaded
	unsigned int textureArrayCount;
	//Data for loaded textures
	vector<unsigned char> textureArrayData;
	//Cube texture height and width
	unsigned int textureArrayHeight;
	unsigned int textureArrayWidth;

	//Tracks loaded textures.
	unordered_map<string, weak_ptr<GraphicsEngineTexture>> loadedTextures;

	//Tracks texture indices available for reuse
	queue<unsigned int> freeTextureIndices;

	//Marks a texture index as available for reuse, removes it from loadedTextures
	void freeTextureFromArray(string name, unsigned int num);

	//Variables to deal with matrix block.
	GLuint matrixBlockBuffer;
	GLint matrixBlockUniformOffsets[2];

	//Variables to deal with light block.
	GLuint lightBlockBuffer;
	GLint lightBlockUniformOffsets[4];

	//Holds data for the engine's point lights
	unsigned short pointLightCount;
	PointLightOffsetData *pointLightOffsets;
	PointLightData *pointLights;

	//Holds data for the engine's spot lights
	unsigned short spotLightCount;
	SpotLightOffsetData *spotLightOffsets;
	SpotLightData *spotLights;

	//Sunlight information
	glm::vec3 sunColor;
	glm::vec3 sunDirection;

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

	//Perform viewport update, if needed
	void updateViewport();

	//Disable copy and move constructor.
	GraphicsEngine(const GraphicsEngine& target) = delete;
	GraphicsEngine(GraphicsEngine&& target) = delete;

	//Sets opengl state and tracks it so it can be later reverted.
	void internalSetEngineParameter(StateVariable param, GraphicsEngineStateVariable);
	void updateModelToCamera();
	void updateCameraToClip();
	glm::vec3 ambientLight;

	void updateSunlight();

public:
	//Constructor. Takes a device context.
	GraphicsEngine(HDC deviceContext);
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

	//Runs an openGL error check and prints errors to the globalLogger.
	bool doErrorCheck();

	//Draws a cylinder at the given coordinate with a given radius and height. Triangles is currently ignored, but will eventually specify the number of triangles to use to draw the cylinder sides.
	//This function does not work, but is being kept in temporarily until the proper replacement model is created.
	void drawCylinder(const glm::dmat4& modelToWorld, double radius, double height, int triangles);

	//Clears the screen
	void clearScreen();

	//Uploads the texture array to OpenGL
	void bindTextureArray();
	//Adds a texture to the texture array
	shared_ptr<GraphicsEngineTexture> loadRawToTextureArray(string textureName, unsigned char* texture, unsigned int width, unsigned int height);
	shared_ptr<GraphicsEngineTexture> loadPNGToTextureArray(string fileName);

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

	//Selects program for use
	void useProgram(ShaderProgram& shaderProgram);

	//To engine matrices
	void setModelToWorld(glm::dmat4 modelToWorld);
	void setWorldToCamera(glm::dmat4 worldToCamera);
	void setCameraToClip(glm::mat4 cameraToClip);

	//Swap front/back buffers
	void swapBuffers();

	//Sets lighting
	void setAmbientLight(glm::vec3 ambientLight);
	void setSunlight(glm::vec3 color, glm::vec3 direction);
	unsigned short getPointLightCount();
	void setPointLight(unsigned int number, bool enabled, glm::vec3 color, glm::vec3 location, glm::vec3 attenuation);
	void setPointLight(unsigned int number, PointLightData data);
	void enablePointLight(unsigned int number);
	void disablePointLight(unsigned int number);
	unsigned short getSpotLightCount();
	void setSpotLight(unsigned int number, bool enabled, glm::vec3 color, glm::vec3 location, glm::vec3 attenuation, glm::vec3 direction, float fullDot, float minDot);
	void setSpotLight(unsigned int number, SpotLightData data);
	void enableSpotLight(unsigned int number);
	void disableSpotLight(unsigned int number);
};

#endif
