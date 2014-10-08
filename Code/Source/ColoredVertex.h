// Name:
// ColoredVertex.h
// Description:
// Header file for ColoredVertex struct
// Notes:
// OS-Unaware
// A ColoredVertex consists of the location of the vertex and the color for the vertex.

#ifndef COLORED_VERTEX_H
#define COLORED_VERTEX_H

#include "Vec3.h"

struct ColoredVertex
{
	Vec3<float> location;
	Vec3<float> normal;
	unsigned char color[4];
	unsigned char diffuse;
	unsigned char specular;
	ColoredVertex& operator+=(Vec3<float> modifier)
	{
		location += modifier;
		return *this;
	};
};

#endif