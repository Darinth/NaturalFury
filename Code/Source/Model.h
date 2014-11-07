// Name:
// Model.h
// Description:
// Header file for Model class.
// Notes:
// OS-Unaware
// Base class for creating of various kinds of 3D models.

#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
using namespace glm;

class Model
{
private:

public:
	virtual void drawFull(dmat4 modelToWorld) = 0;
	virtual void drawShadows(dmat4 modelToWorld) = 0;
	virtual bool castsShadows() = 0;
};

#endif