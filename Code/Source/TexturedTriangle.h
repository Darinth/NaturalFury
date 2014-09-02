// Name:
// TexturedTriangle.h
// Description:
// Header file for TexturedTriangle struct
// Notes:
// OS-Unaware
// A TexturedTriangle contains three textured vertices that represent the points of the triangle.

#ifndef TEXTUREDTRIANGLE_H
#define TEXTUREDTRIANGLE_H

#include "TexturedVertex.h"
#include "Vec3.h"

struct TexturedTriangle
{
	TexturedVertex point0;
	TexturedVertex point1;
	TexturedVertex point2;
	TexturedTriangle& operator+=(Vec3<float> modifier)
	{
		point0 += modifier;
		point1 += modifier;
		point2 += modifier;
		return *this;
	};
};

#endif