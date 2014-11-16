// Name:
// AmbientQuadModel.h
// Description:
// Header file for AmbientQuadModel.h
// Full-screen quad used for the ambient lighting pass.
// Notes:
// OS-Unaware
// TODO: This really shouldn't be a model, remove the parent class.

#ifndef AMBIENT_QUAD_MODEL_H
#define AMBIENT_QUAD_MODEL_H

#include <memory>
using namespace std;

#include <glm\glm.hpp>

#include "Model.h"

class GraphicsEngine;
class GraphicsEngineTexture;

class AmbientQuadModel : public Model
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
	AmbientQuadModel(GraphicsEngine* graphicsEngine);
	AmbientQuadModel(AmbientQuadModel&) = delete;
	void operator=(AmbientQuadModel&) = delete;
	~AmbientQuadModel();
	//Draw the model
	void drawFull(glm::dmat4 modelToWorld);
	void drawShadows(glm::dmat4 modelToWorld);
	void draw();
	//because why not, this'll be changed when we change the inheritance.
	virtual bool castsShadows() { return true; }
};

#endif
