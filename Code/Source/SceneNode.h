// Name:
// SceneNode.h
// Description:
// Header file for SceneNode.h
// SceneNode basic class designed to be straight-forward scene node, no children: it just draws the specified model with the specified matrix.
// Notes:
// OS-Unaware

#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include "ISceneNode.h"

#include <memory>
using namespace std;

#include <glm\glm.hpp>

class Model;

class SceneNode : public ISceneNode
{
private:
public:
	SceneNode();
	SceneNode(shared_ptr<Model> model, glm::dmat4 modelToWorld);
	glm::dmat4 modelToWorld;
	shared_ptr<Model> model;
	virtual void draw();
};

#endif
