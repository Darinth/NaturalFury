// Name:
// CubeModel.h
// Description:
// Header file for CubeModel.h
// A CubeModel is a hard-coded model of cube.
// Notes:
// OS-Unaware

#ifndef CUBE_MODEL_H
#define CUBE_MODEL_H

#include <memory>
using namespace std;

#include <glm\glm.hpp>

#include "Model.h"

class GraphicsEngine;
class GraphicsEngineTexture;

class CubeModel : public Model
{
private:
	//Graphics engine used for drawing
	GraphicsEngine* graphicsEngine;
	//VAO used for drawing
	unsigned int VAO;
	//Point buffer
	unsigned int buffer;
	void draw(const glm::dmat4 &modelToWorld);
public:
	CubeModel(GraphicsEngine* graphicsEngine, const glm::vec3& halfExtents,
		shared_ptr<GraphicsEngineTexture> topTexture, shared_ptr<GraphicsEngineTexture> northTexture, shared_ptr<GraphicsEngineTexture> eastTexture,
		shared_ptr<GraphicsEngineTexture> southTexture, shared_ptr<GraphicsEngineTexture> westTexture, shared_ptr<GraphicsEngineTexture> bottomTexture);
	CubeModel(CubeModel&) = delete;
	void operator=(CubeModel&) = delete;
	~CubeModel();
	//Draw the model
	void drawFull(glm::dmat4 modelToWorld);
	void drawShadows(glm::dmat4 modelToWorld);
	//Yes, cubes cast shadows.
	virtual bool castsShadows() { return true; }
};

#endif
