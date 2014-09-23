// Name:
// ColoredTriangle.h
// Description:
// Header file for ColoredTriangle struct
// Notes:
// OS-Unaware
// A ColoredTriangle contains three colored vertices that represent the points of the triangle.

#ifndef COLORED_TRIANGLE_H
#define COLORED_TRIANGLE_H

#include "ColoredVertex.h"
#include "Vec3.h"

struct ColoredTriangle
{
	ColoredVertex point0;
	ColoredVertex point1;
	ColoredVertex point2;
	ColoredTriangle& operator+=(Vec3<float> modifier)
	{
		point0 += modifier;
		point1 += modifier;
		point2 += modifier;
		return *this;
	};
};

#endif