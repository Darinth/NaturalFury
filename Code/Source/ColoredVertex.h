// Name:
// ColoredVertex.h
// Description:
// Header file for ColoredVertex struct
// Notes:
// OS-Unaware
// A ColoredVertex consists of the location of the vertex and the color for the vertex.

#ifndef COLOREDVERTEX_H
#define COLOREDVERTEX_H

#include "Vec3.h"

struct ColoredVertex
{
	Vec3<float> location;
	Vec3<float> color;
	ColoredVertex& operator+=(Vec3<float> modifier)
	{
		location += modifier;
		return *this;
	};
};

#endif