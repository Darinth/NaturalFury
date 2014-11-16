// Name:
// AmbientCubeModel.cpp
// Description:
// Implementation file for AmbientCubeModel class.
// Notes:
// OS-Unaware
// See AmbientCubeModel.h for class usage details

#include "CustomMemory.h"

#include "AmbientQuadModel.h"

#include <glm/glm.hpp>

#include "GraphicsEngine.h"
#include "ColoredTriangle.h"
#include "GraphicsEngineTexture.h"

//Point list for drawing
unsigned short quadBuffer[] = { 0, 1, 2, 3 };

AmbientQuadModel::AmbientQuadModel(GraphicsEngine *graphicsEngine) : graphicsEngine(graphicsEngine)
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to construct a CubeModel with unclaimed GraphicsEngine");

	//Vertices for the quad, provided in clip space.
	glm::vec3 cubeVertices[4] =
	{
		glm::vec3( -1.0, -1.0, 0.0 ),  //1
		glm::vec3( 1.0, -1.0, 0.0 ),  //2
		glm::vec3( 1.0, 1.0, 0.0 ),  //3
		glm::vec3( -1.0, 1.0, 0.0 )   //4
	};

	//Create VAO and buffer
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_DRAW);

	//Setup VAO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, 0);
}

AmbientQuadModel::~AmbientQuadModel()
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to deconstruct a CubeModel with unclaimed GraphicsEngine");

	//Delete VAO and buffers
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &buffer);
}

void AmbientQuadModel::draw(const glm::dmat4 &modelToWorld)
{

}

void AmbientQuadModel::drawFull(glm::dmat4 modelToWorld)
{

}

void AmbientQuadModel::drawShadows(glm::dmat4 modelToWorld)
{

}

void AmbientQuadModel::draw()
{
	//Bind the VAO
	glBindVertexArray(VAO);
	//Draw the stuff
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, quadBuffer);
	//Check for errors in the openGL state
	graphicsEngine->doErrorCheck();
}