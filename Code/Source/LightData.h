// Name:
// LightData.h
// Description:
// Header file for LightData.h
// LightData holds information used by OpenGL to draw lights.
// Notes:
// OS-Unaware

#ifndef I_SCENE_NODE_H
#define I_SCENE_NODE_H

#include <glm/glm.hpp>

struct LightData
{
	glm::vec3 direction;
	float power;
};

#endif