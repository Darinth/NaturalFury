// Name:
// CubeModel.cpp
// Description:
// Implementation file for CubeModel class.
// Notes:
// OS-Unaware
// See CubeModel.h for class usage details

#include "CustomMemory.h"

#include "CubeModel.h"

#include "GraphicsEngine.h"
#include "ColoredTriangle.h"

const ColoredTriangle coloredCubeVertices[] =
{
	// Top
	{
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, 1.0f) },
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, 1.0f) },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, 1.0f) }
	},

	{
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, 1.0f) },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, 1.0f) },
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, 1.0f) }
	},

	// Front
	{
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, 1.0f, 1.0f) },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, 1.0f, 1.0f) },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, 1.0f, 1.0f) }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, 1.0f, 0.0f) },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, 1.0f, 0.0f) },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, 0.0f, -1.0f), Vec3<float>(0.0f, 1.0f, 0.0f) }
	},

	// Right
	{
		{ Vec3<float>(1.0f, 1.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 1.0f) },
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 1.0f) },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 1.0f) }
	},

	{
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 1.0f) },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 1.0f) },
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 1.0f) }
	},

	// Back
	{
		{ Vec3<float>(1.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 1.0f, 0.0f) },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 1.0f, 0.0f) },
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 1.0f, 0.0f) }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 1.0f, 0.0f) },
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 1.0f, 0.0f) },
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(1.0f, 1.0f, 0.0f) }
	},

	// Left
	{
		{ Vec3<float>(0.0f, 1.0f, 1.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f) },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f) },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f) }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f) },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f) },
		{ Vec3<float>(0.0f, 1.0f, 0.0f), Vec3<float>(-1.0f, 0.0f, 0.0f), Vec3<float>(1.0f, 0.0f, 0.0f) }
	},

	// Bottom
	{
		{ Vec3<float>(1.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f) },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f) },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f) }
	},

	{
		{ Vec3<float>(0.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f) },
		{ Vec3<float>(1.0f, 0.0f, 0.0f), Vec3<float>(0.0f, -1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f) },
		{ Vec3<float>(0.0f, 0.0f, 1.0f), Vec3<float>(0.0f, -1.0f, 0.0f), Vec3<float>(0.0f, 1.0f, 0.0f) }
	}
};


CubeModel::CubeModel(GraphicsEngine *graphicsEngine) : graphicsEngine(graphicsEngine)
{
	if (!graphicsEngine->isClaimed())
		throw exception("Attempt to construct a ShaderProgram with unclaimed GraphicsEngine");

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void *)(offsetof(ColoredVertex, normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void *)(offsetof(ColoredVertex, color)));
}