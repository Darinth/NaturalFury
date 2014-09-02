// Name:
// TexturedVertex.h
// Description:
// Header file for TexturedVertex struct
// Notes:
// OS-Unaware
// A TexturedVertex consists of the location of the vertex, the texture number for the vertex, and the texture coordinates used by the vertex.

#ifndef TEXTUREDVERTEX_H
#define TEXTUREDVERTEX_H

#include "Vec3.h"

struct TexturedVertex
{
	Vec3<float> location;
	unsigned char texture;
	unsigned char textureX;
	unsigned char textureY;
	TexturedVertex& operator+=(Vec3<float> modifier)
	{
		location += modifier;
		return *this;
	};
};

#endif