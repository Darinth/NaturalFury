// Name:
// CubeModel.cpp
// Description:
// Implementation file for CubeModel class.
// Notes:
// OS-Unaware
// See CubeModel.h for class usage details

#include "CustomMemory.h"

#include "CubeModel.h"

#include <glm/glm.hpp>

#include "GraphicsEngine.h"
#include "ColoredTriangle.h"

const ColoredVertex coloredCubeVertices[] =
{
	// Top
	{ Vec3<float>(0.5f, 0.5f, -0.5f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 255, 255, 255, 255 }, //1
	{ Vec3<float>(0.5f, 0.5f, 0.5f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 255, 255, 255, 255 },  //2
	{ Vec3<float>(-0.5f, 0.5f, 0.5f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 255, 255, 255, 255 },  //3
	{ Vec3<float>(-0.5f, 0.5f, -0.5f), Vec3<float>(0.0f, 1.0f, 0.0f), 0, 0, 255, 255, 255, 255 }, //4

	// Front
	{ Vec3<float>(0.5f, -0.5f, -0.5f), Vec3<float>(0.0f, 0.0f, -1.0f), 0, 255, 255, 255, 255, 255 }, //5
	{ Vec3<float>(0.5f, 0.5f, -0.5f), Vec3<float>(0.0f, 0.0f, -1.0f), 0, 255, 255, 255, 255, 255 },  //6
	{ Vec3<float>(-0.5f, 0.5f, -0.5f), Vec3<float>(0.0f, 0.0f, -1.0f), 0, 255, 255, 255, 255, 255 },  //7
	{ Vec3<float>(-0.5f, -0.5f, -0.5f), Vec3<float>(0.0f, 0.0f, -1.0f), 0, 255, 255, 255, 255, 255 }, //8

	// Right
	{ Vec3<float>(0.5f, 0.5f, 0.5f), Vec3<float>(1.0f, 0.0f, 0.0f), 255, 0, 255, 255, 255, 255 },  //9
	{ Vec3<float>(0.5f, 0.5f, -0.5f), Vec3<float>(1.0f, 0.0f, 0.0f), 255, 0, 255, 255, 255, 255 }, //10
	{ Vec3<float>(0.5f, -0.5f, -0.5f), Vec3<float>(1.0f, 0.0f, 0.0f), 255, 0, 255, 255, 255, 255 }, //11
	{ Vec3<float>(0.5f, -0.5f, 0.5f), Vec3<float>(1.0f, 0.0f, 0.0f), 255, 0, 255, 255, 255, 255 },  //12

	// Back
	{ Vec3<float>(-0.5f, 0.5f, 0.5f), Vec3<float>(0.0f, 0.0f, 1.0f), 255, 255, 0, 255, 255, 255 },  //13
	{ Vec3<float>(0.5f, 0.5f, 0.5f), Vec3<float>(0.0f, 0.0f, 1.0f), 255, 255, 0, 255, 255, 255 },   //14
	{ Vec3<float>(0.5f, -0.5f, 0.5f), Vec3<float>(0.0f, 0.0f, 1.0f), 255, 255, 0, 255, 255, 255 },   //15
	{ Vec3<float>(-0.5f, -0.5f, 0.5f), Vec3<float>(0.0f, 0.0f, 1.0f), 255, 255, 0, 255, 255, 255 }, //16

	// Left
	{ Vec3<float>(-0.5f, 0.5f, -0.5f), Vec3<float>(-1.0f, 0.0f, 0.0f), 255, 0, 0, 255, 255, 255 }, //17
	{ Vec3<float>(-0.5f, 0.5f, 0.5f), Vec3<float>(-1.0f, 0.0f, 0.0f), 255, 0, 0, 255, 255, 255 },  //18
	{ Vec3<float>(-0.5f, -0.5f, 0.5f), Vec3<float>(-1.0f, 0.0f, 0.0f), 255, 0, 0, 255, 255, 255 },  //19
	{ Vec3<float>(-0.5f, -0.5f, -0.5f), Vec3<float>(-1.0f, 0.0f, 0.0f), 255, 0, 0, 255, 255, 255 }, //20

	// Bottom
	{ Vec3<float>(-0.5f, -0.5f, 0.5f), Vec3<float>(0.0f, -1.0f, 0.0f), 0, 255, 0, 255, 255, 255 }, //21
	{ Vec3<float>(0.5f, -0.5f, 0.5f), Vec3<float>(0.0f, -1.0f, 0.0f), 0, 255, 0, 255, 255, 255 },  //22
	{ Vec3<float>(0.5f, -0.5f, -0.5f), Vec3<float>(0.0f, -1.0f, 0.0f), 0, 255, 0, 255, 255, 255 },  //23
	{ Vec3<float>(-0.5f, -0.5f, -0.5f), Vec3<float>(0.0f, -1.0f, 0.0f), 0, 255, 0, 255, 255, 255 } //24
};

unsigned short cubeBuffer[] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};


CubeModel::CubeModel(GraphicsEngine *graphicsEngine) : graphicsEngine(graphicsEngine)
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to construct a CubeModel with unclaimed GraphicsEngine");

	//Create VAO and buffer
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(coloredCubeVertices), coloredCubeVertices, GL_DYNAMIC_DRAW);

	//Setup VAO
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void *)(offsetof(ColoredVertex, normal)));
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColoredVertex), (void *)(offsetof(ColoredVertex, color)));
	glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColoredVertex), (void *)(offsetof(ColoredVertex, diffuse)));
	glVertexAttribPointer(4, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColoredVertex), (void *)(offsetof(ColoredVertex, specular)));
}

CubeModel::~CubeModel()
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to deconstruct a CubeModel with unclaimed GraphicsEngine");

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &buffer);
}

void CubeModel::draw(glm::dmat4 modelToWorld)
{
	graphicsEngine->setModelToWorld(modelToWorld);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, cubeBuffer);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	graphicsEngine->doErrorCheck();
}
