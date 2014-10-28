// Name:
// TexturedVertex.h
// Description:
// Header file for TexturedVertex struct
// Notes:
// OS-Unaware
// A TexturedVertex consists of the location of the vertex, the texture number for the vertex, and the texture coordinates used by the vertex.

#ifndef TEXTURED_VERTEX_H
#define TEXTURED_VERTEX_H

#include "Vec3.h"

struct TexturedVertex
{
	//Location of the vertex
	Vec3<float> location;
	//Normal for the vertex
	Vec3<float> normal;
	//Texture number
	unsigned char texture;
	//Texture Coordinates
	unsigned char textureX;
	unsigned char textureY;
	//Diffuse and specular lighting components
	unsigned char diffuse;
	unsigned char specular;
	TexturedVertex& operator+=(Vec3<float> modifier)
	{
		location += modifier;
		return *this;
	};
};

#endif