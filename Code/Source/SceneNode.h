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

#include "Model.h"

class SceneNode : public ISceneNode
{
private:
public:
	//Default constructor and constructor the supply a model and modelToWorld matrix
	SceneNode();
	SceneNode(shared_ptr<Model> model, glm::dmat4 modelToWorld);
	//model and modelToWorld matrix
	glm::dmat4 modelToWorld;
	shared_ptr<Model> model;
	//Draw the model.
	virtual void draw();
	//Return if the model casts shadows.
	virtual bool castsShadows() { return model->castsShadows(); }
};

#endif
