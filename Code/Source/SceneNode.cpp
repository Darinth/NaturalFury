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

void SceneNode::draw()
{
	model->draw(ModelToWorld);
}
