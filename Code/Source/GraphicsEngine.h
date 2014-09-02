// Name:
// GraphicsEngine.h
// Description:
// Header file for GraphicsEngine class.
// Notes:
// OS-Aware
// Sets up OpenGL context and provides functions for accessing portions of it in a controller fashion.

#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include <Windows.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <glload/gl_3_3.h>
#include <glload/gll.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>
using namespace std;

#include "Vec3.h"
#include "TexturedTriangle.h"
#include "Lockable.h"

//Triangles used for a typical cube.
extern const TexturedTriangle cubeVertices[];
//Vertices used for drawing a cylinder
extern const TexturedVertex cylinderVertices[];
class ThreadSafeFstream;
class GraphicsEngine : public Lockable
{
private:

	//DeviceContext
	HDC deviceContext;
	//RenderingContext
	HGLRC renderingContext;

	//Variables for setting up camera to clip matrix
	const float zNear; //Closest visible point
	const float zFar;  //Furthest visible point
	const float frustumScale;  //Increases/decreases zoom
	float screenRatio;   //Aspect ratio of viewport

	//Point of view variables. X, Y, Z coordinates and rotations around X and Y axis.
	double PoVX;
	double PoVY;
	double PoVZ;
	float rotationY;
	float rotationX;

	//VAO and Buffer for drawing plain old cylinder.
	//Used as placeholder for stuff there is no model for yet.
	GLuint cylinderVAO;
	GLuint cylinderBuffer;

	//Matrix for transformation from camera to clip space
	glm::mat4 cameraToClip;

	//Shader uniform for camera to clip matrix
	GLuint cameraToClipMatrixUniform;
	//Shader uniform for model to camera matrix
	GLuint modelToCameraMatrixUniform;
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

	//The main shader program used for pretty much everything
	unsigned int theProgram;

	//Shader program used specifically to draw lines (currently used for debug drawing)
	unsigned int lineProgram;

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
	GraphicsEngine(const GraphicsEngine& target);
	GraphicsEngine(GraphicsEngine&& target);

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

	//Sets the PoV for standard program drawing
	void setPOV(double x, double y, double z, float angDegZ, float angDegX);
	//Getters for PoV variables
	double getPoVX() const;
	double getPoVY() const;
	double getPoVZ() const;
	float getRotationY() const;
	float getRotationX() const;
	bool doErrorCheck();

	//Preps OpenGL to perform drawing using the given model to camera matrix and VAO.
	void prepStandardProgramDraw(const float* matrix, unsigned int VAO);
	void prepColoredProgramDraw(const float* matrix, unsigned int VAO);

	//Draws a cylinder at the given coordinate with a given radius and height. Triangles is currently ignored, but will eventually specify the number of triangles to use to draw the cylinder sides.
	void drawCylinder(Vec3<double> baseCoord, double radius, double height, int triangles);

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
};

#endif