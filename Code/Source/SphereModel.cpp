// Name:
// SphereModel.cpp
// Description:
// Implementation file for SphereModel class.
// Notes:
// OS-Unaware
// See SphereModel.h for class usage details

#include "CustomMemory.h"

#include "SphereModel.h"

#include <glm/glm.hpp>

#include "GraphicsEngine.h"
#include "ColoredTriangle.h"
#include "GraphicsEngineTexture.h"

//Point list for drawing
extern unsigned short cubeBuffer[];

SphereModel::SphereModel(GraphicsEngine *graphicsEngine) : graphicsEngine(graphicsEngine)
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to construct a CubeModel with unclaimed GraphicsEngine");

	//Consts for different lighting components
	const unsigned char diffuseComponent = 128;
	const unsigned char specularComponent = 128;

	//Vertices for the different parts of the cube
	/*TexturedVertex cubeVertices[24] =
	{
		// Top
		{ Vec3<float>(halfExtents.x, halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, 1.0f, 0.0f), topTexture->textureNum, 255, 255, diffuseComponent, specularComponent },  //1
		{ Vec3<float>(halfExtents.x, halfExtents.y, halfExtents.z), Vec3<float>(0.0f, 1.0f, 0.0f), topTexture->textureNum, 255, 0, diffuseComponent, specularComponent },   //2
		{ Vec3<float>(-halfExtents.x, halfExtents.y, halfExtents.z), Vec3<float>(0.0f, 1.0f, 0.0f), topTexture->textureNum, 0, 0, diffuseComponent, specularComponent },  //3
		{ Vec3<float>(-halfExtents.x, halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, 1.0f, 0.0f), topTexture->textureNum, 0, 255, diffuseComponent, specularComponent }, //4

		// South
		{ Vec3<float>(halfExtents.x, -halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, 0.0f, -1.0f), southTexture->textureNum, 0, 255, diffuseComponent, specularComponent },  //5
		{ Vec3<float>(halfExtents.x, halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, 0.0f, -1.0f), southTexture->textureNum, 0, 0, diffuseComponent, specularComponent },   //6
		{ Vec3<float>(-halfExtents.x, halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, 0.0f, -1.0f), southTexture->textureNum, 255, 0, diffuseComponent, specularComponent },  //7
		{ Vec3<float>(-halfExtents.x, -halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, 0.0f, -1.0f), southTexture->textureNum, 255, 255, diffuseComponent, specularComponent }, //8

		// East
		{ Vec3<float>(halfExtents.x, halfExtents.y, halfExtents.z), Vec3<float>(1.0f, 0.0f, 0.0f), eastTexture->textureNum, 255, 0, diffuseComponent, specularComponent },   //9
		{ Vec3<float>(halfExtents.x, halfExtents.y, -halfExtents.z), Vec3<float>(1.0f, 0.0f, 0.0f), eastTexture->textureNum, 0, 0, diffuseComponent, specularComponent },  //10
		{ Vec3<float>(halfExtents.x, -halfExtents.y, -halfExtents.z), Vec3<float>(1.0f, 0.0f, 0.0f), eastTexture->textureNum, 0, 255, diffuseComponent, specularComponent }, //11
		{ Vec3<float>(halfExtents.x, -halfExtents.y, halfExtents.z), Vec3<float>(1.0f, 0.0f, 0.0f), eastTexture->textureNum, 255, 255, diffuseComponent, specularComponent },  //12

		// North
		{ Vec3<float>(-halfExtents.x, halfExtents.y, halfExtents.z), Vec3<float>(0.0f, 0.0f, 1.0f), northTexture->textureNum, 255, 0, diffuseComponent, specularComponent },  //13
		{ Vec3<float>(halfExtents.x, halfExtents.y, halfExtents.z), Vec3<float>(0.0f, 0.0f, 1.0f), northTexture->textureNum, 0, 0, diffuseComponent, specularComponent },   //14
		{ Vec3<float>(halfExtents.x, -halfExtents.y, halfExtents.z), Vec3<float>(0.0f, 0.0f, 1.0f), northTexture->textureNum, 0, 255, diffuseComponent, specularComponent },  //15
		{ Vec3<float>(-halfExtents.x, -halfExtents.y, halfExtents.z), Vec3<float>(0.0f, 0.0f, 1.0f), northTexture->textureNum, 255, 255, diffuseComponent, specularComponent }, //16

		// West
		{ Vec3<float>(-halfExtents.x, halfExtents.y, -halfExtents.z), Vec3<float>(-1.0f, 0.0f, 0.0f), westTexture->textureNum, 255, 0, diffuseComponent, specularComponent },  //17
		{ Vec3<float>(-halfExtents.x, halfExtents.y, halfExtents.z), Vec3<float>(-1.0f, 0.0f, 0.0f), westTexture->textureNum, 0, 0, diffuseComponent, specularComponent },   //18
		{ Vec3<float>(-halfExtents.x, -halfExtents.y, halfExtents.z), Vec3<float>(-1.0f, 0.0f, 0.0f), westTexture->textureNum, 0, 255, diffuseComponent, specularComponent },  //19
		{ Vec3<float>(-halfExtents.x, -halfExtents.y, -halfExtents.z), Vec3<float>(-1.0f, 0.0f, 0.0f), westTexture->textureNum, 255, 255, diffuseComponent, specularComponent }, //20

		// Bottom
		{ Vec3<float>(-halfExtents.x, -halfExtents.y, halfExtents.z), Vec3<float>(0.0f, -1.0f, 0.0f), bottomTexture->textureNum, 255, 255, diffuseComponent, specularComponent }, //21
		{ Vec3<float>(halfExtents.x, -halfExtents.y, halfExtents.z), Vec3<float>(0.0f, -1.0f, 0.0f), bottomTexture->textureNum, 0, 255, diffuseComponent, specularComponent },  //22
		{ Vec3<float>(halfExtents.x, -halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, -1.0f, 0.0f), bottomTexture->textureNum, 0, 0, diffuseComponent, specularComponent }, //23
		{ Vec3<float>(-halfExtents.x, -halfExtents.y, -halfExtents.z), Vec3<float>(0.0f, -1.0f, 0.0f), bottomTexture->textureNum, 255, 0, diffuseComponent, specularComponent } //24
	};*/

	//Create VAO and buffer
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_DRAW);

	//Setup VAO
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)(offsetof(TexturedVertex, normal)));
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(TexturedVertex), (void *)(offsetof(TexturedVertex, texture)));
	glVertexAttribPointer(3, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TexturedVertex), (void *)(offsetof(TexturedVertex, textureX)));
	glVertexAttribPointer(4, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TexturedVertex), (void *)(offsetof(TexturedVertex, diffuse)));
	glVertexAttribPointer(5, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TexturedVertex), (void *)(offsetof(TexturedVertex, specular)));
}

SphereModel::~SphereModel()
{
	graphicsEngine->assertClaimed("Deconstructing SphereModel.");

	//Delete VAO and buffers
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &buffer);
}

void SphereModel::draw(const glm::dmat4 &modelToWorld)
{
	//Set the ModelToWorld translation
	graphicsEngine->setModelToWorld(modelToWorld);
	//Bind the VAO
	glBindVertexArray(VAO);
	//Draw the stuff
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, cubeBuffer);
	//Check for errors in the openGL state
	graphicsEngine->doErrorCheck();
}

void SphereModel::drawFull(glm::dmat4 modelToWorld)
{
	draw(modelToWorld);
}

void SphereModel::drawShadows(glm::dmat4 modelToWorld)
{
	draw(modelToWorld);
}
