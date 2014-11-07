// Name:
// SceneNode.cpp
// Description:
// Implementation file for SceneNode class.
// Notes:
// OS-Unaware
// See SceneNode.h for class usage details

#include "CustomMemory.h"

#include "SceneNode.h"

#include "Model.h"

SceneNode::SceneNode()
{

}

SceneNode::SceneNode(shared_ptr<Model> model, glm::dmat4 modelToWorld) : model(model), modelToWorld(modelToWorld)
{
}

void SceneNode::drawFull() const
{
	//Draw the model using the matrix!
	model->drawFull(modelToWorld);
}

void SceneNode::drawShadows() const
{
	//Draw the model using the matrix!
	model->drawShadows(modelToWorld);
}
