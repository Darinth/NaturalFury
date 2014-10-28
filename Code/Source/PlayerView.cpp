// Name:
// PlayerView.cpp
// Description:
// Implementation Class for PlayerView class
// Notes:
// OS-Unaware

#include "CustomMemory.h"

#include "PlayerView.h"

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vec3.h"
#include "GameEngine.h"
#include "ShaderProgram.h"
#include "CubeModel.h"
#include "Scene.h"
#include "Globals.h"
#include "ResourceCache.h"
#include "SceneNode.h"
#include "GraphicsEngine.h"

PlayerView::PlayerView() : gameWindow(nullptr), done(false), debugCameraRotation(0), debugCameraTilt(0), sunColor(1.0f, 1.0f, 1.0f), shiftDown(false), debugCameraPosition(0, 3, 0)
{
}

PlayerView::~PlayerView()
{
}

void PlayerView::keyDown(KeyEnum key)
{
	//If the shift key isn't pressed, perform standard key processing
	if (shiftDown == false)
	{
		//Mark when shift key is pressed
		if (key == KeyEnum::Leftshift)
		{
			shiftDown = true;
		}
		//Shift camera for WASD camera control
		else if (key == KeyEnum::W)
			shiftDebugCamera(glm::dvec3(0.0, 0.0, 1.0));
		else if (key == KeyEnum::A)
			shiftDebugCamera(glm::dvec3(-1.0, 0.0, 0.0));
		else if (key == KeyEnum::S)
			shiftDebugCamera(glm::dvec3(0.0, 0.0, -1.0));
		else if (key == KeyEnum::D)
			shiftDebugCamera(glm::dvec3(1.0, 0.0, 0.0));
	}
	else
	{
		//Use ASD for sun control when shift is pressed
		if (key == KeyEnum::A)
		{
			sunColor.r += 0.1f;
			if (sunColor.r > 1.05)
				sunColor.r = 0.0;
		}
		else if (key == KeyEnum::S)
		{
			sunColor.g += 0.1f;
			if (sunColor.g > 1.05)
				sunColor.g = 0.0;
		}
		else if (key == KeyEnum::D)
		{
			sunColor.b += 0.1f;
			if (sunColor.b > 1.05)
				sunColor.b = 0.0;
		}
	}
}

void PlayerView::keyUp(KeyEnum key)
{
	//Mark when shift key is released
	if (key == KeyEnum::Leftshift)
	{
		shiftDown = false;
	}
}

void PlayerView::mouseMove(long distanceX, long distanceY)
{
	//Use mouse movement to rotate/tilt camera
	rotateDebugCamera(distanceX);
	tiltDebugCamera(distanceY);
}

void PlayerView::mouseDown(int button)
{
	//Zoom in(ish)!
	if (button == 1)
		shiftDebugCamera(glm::dvec3(0.0, 0.0, 1.0));
}

void PlayerView::mouseUp(int button)
{
	//Zoom out(ish)!
	if (button == 1)
		shiftDebugCamera(glm::dvec3(0.0, 0.0, -1.0));
}

void PlayerView::gamePadButtonDown(int number)
{

}

void PlayerView::gamePadButtonUp(int number)
{

}

void PlayerView::gamePadThrottleChange(int number, double value)
{

}

void PlayerView::windowClose()
{
	//Once the window is closed, we're done.
	done = true;
}

void PlayerView::shiftDebugCamera(glm::dvec3 shift)
{
	using namespace glm::gtc::matrix_transform;
	dvec4 finalShift = dvec4(shift, 1.0) * rotate(rotate(glm::dmat4(1.0), debugCameraTilt, dvec3(1.0, 0.0, 0.0)), debugCameraRotation, dvec3(0.0, -1.0, 0.0));
	debugCameraPosition += dvec3(finalShift);
}

void PlayerView::rotateDebugCamera(double angle)
{
	debugCameraRotation += angle;
}

void PlayerView::tiltDebugCamera(double angle)
{
	debugCameraTilt += angle;
}

void PlayerView::updateScene(Scene* scene)
{
	//Generate WorldToCamera transform, send it to graphicsEngine, clear the screen, draw the scene, swap the buffer.
	glm::dmat4 debugCameraTransform(1.0);
	debugCameraTransform = glm::gtc::matrix_transform::rotate(debugCameraTransform, debugCameraTilt, glm::dvec3(1.0, 0.0, 0.0));
	debugCameraTransform = glm::gtc::matrix_transform::rotate(debugCameraTransform, debugCameraRotation, glm::dvec3(0.0, -1.0, 0.0));
	debugCameraTransform = glm::gtc::matrix_transform::translate(debugCameraTransform, glm::dvec3(-debugCameraPosition.x, -debugCameraPosition.y, -debugCameraPosition.z));
	graphicsEngine->setWorldToCamera(debugCameraTransform);

	graphicsEngine->clearScreen();
	scene->draw();
	graphicsEngine->swapBuffers();
}


void PlayerView::run(Window* gameWindow, GameEngine* gameEngine)
{
	//Store game window.
	this->gameWindow = gameWindow;
	//Store graphicsEngine
	this->graphicsEngine = gameWindow->getGraphicsEngine();

	//Set the UI for the gameWindow to this.
	gameWindow->setUserInterface(this);

	//Create shader
	shared_ptr<ResourceHandle> vertShader = globalResourceCache->gethandle("TextureShader.vert");
	shared_ptr<ResourceHandle> fragShader = globalResourceCache->gethandle("TextureShader.frag");

	ShaderProgram shader(graphicsEngine, vertShader, fragShader);

	//Load textures
	shared_ptr<GraphicsEngineTexture> stoneTexture = graphicsEngine->loadPNGToTextureArray("stone.png");

	shared_ptr<GraphicsEngineTexture> grassTopTexture = graphicsEngine->loadPNGToTextureArray("grassTop.png");

	shared_ptr<GraphicsEngineTexture> grassSideTexture = graphicsEngine->loadPNGToTextureArray("grassSide.png");

	shared_ptr<GraphicsEngineTexture> dirtTexture = graphicsEngine->loadPNGToTextureArray("dirt.png");

	graphicsEngine->bindTextureArray();

	//Create a model for the floor
	shared_ptr<Model> floorModel = shared_ptr<CubeModel>(new CubeModel(graphicsEngine, glm::vec3(100.0f, 0.5f, 100.0f), stoneTexture, stoneTexture, stoneTexture, stoneTexture, stoneTexture, stoneTexture));

	//Create a model of a grass cube
	shared_ptr<Model> cubeModel = shared_ptr<CubeModel>(new CubeModel(graphicsEngine, glm::vec3(0.5f, 0.5f, 0.5f), grassTopTexture, grassSideTexture, grassSideTexture, grassSideTexture, grassSideTexture, dirtTexture));

	//Create a scene
	Scene scene(graphicsEngine);

	//Create scene nodes for the floor and two blocks
	glm::dmat4 modelToWorld(1.0);
	modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(0, -0.5, 0));
	scene.addNode(shared_ptr<SceneNode>(new SceneNode(floorModel, modelToWorld)));
	modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(-1.5, 1, 2));
	scene.addNode(shared_ptr<SceneNode>(new SceneNode(cubeModel, modelToWorld)));
	modelToWorld = glm::gtc::matrix_transform::translate(glm::dmat4(1.0), glm::dvec3(-1.5, 1, 10));
	scene.addNode(shared_ptr<SceneNode>(new SceneNode(cubeModel, modelToWorld)));

	//Until we're done, just draw the scene repeatedly.
	while (!done)
	{
		graphicsEngine->setSunlight(sunColor, glm::vec3(0.0, -1.0, 0.0));
		updateScene(&scene);
	}

	//Clear the user interface from the gameWindow
	gameWindow->setUserInterface(nullptr);

	//Clear our pointers to the graphicsEngine and gameWindow
	this->graphicsEngine = nullptr;
	this->gameWindow = nullptr;

	//Shut down gameEngine
	gameEngine->sendMsg(shared_ptr<EngineMsg>(new EngineMsgShutdown()));
}
