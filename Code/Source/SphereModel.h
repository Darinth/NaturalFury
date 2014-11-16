// Name:
// SphereModel.h
// Description:
// Header file for SphereModel.h
// Model for sphere of arbitrary radius
// Notes:
// OS-Unaware

#ifndef SPHERE_MODEL_H
#define SPHERE_MODEL_H

#include <memory>
using namespace std;

#include <glm\glm.hpp>

#include "Model.h"

class GraphicsEngine;
class GraphicsEngineTexture;

class SphereModel : public Model
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
	SphereModel(GraphicsEngine* graphicsEngine);
	SphereModel(SphereModel&) = delete;
	void operator=(SphereModel&) = delete;
	~SphereModel();
	//Draw the model
	void drawFull(glm::dmat4 modelToWorld);
	void drawShadows(glm::dmat4 modelToWorld);
	//Yes, spheres cast shadows.
	virtual bool castsShadows() { return true; }
};

#endif
