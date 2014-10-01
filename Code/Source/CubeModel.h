// Name:
// CubeModel.h
// Description:
// Header file for CubeModel.h
// A CubeModel is a hard-coded model of cube.
// Notes:
// OS-Unaware

#ifndef CUBE_MODEL_H
#define CUBE_MODEL_H

#include "Model.h"

class GraphicsEngine;

class CubeModel : public Model
{
private:
	GraphicsEngine* graphicsEngine;
	unsigned int VAO;
	unsigned int buffer;
public:
	CubeModel(GraphicsEngine* graphicsEngine);
	CubeModel(CubeModel&) = delete;
	CubeModel(CubeModel&&) = delete;
	~CubeModel();
	void draw(glm::dmat4 modelToWorld);
};

#endif
