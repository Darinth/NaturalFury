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

void SceneNode::draw()
{
	model->draw(modelToWorld);
}

bool SceneNode::castsShadows()
{
	return model->castsShadows();
}