// Name:
// Scene.h
// Description:
// Header file for Scene.h
// A scene is a container for scene nodes. It takes care of drawing all of the nodes in it's list.
// Notes:
// OS-Unaware

#ifndef SCENE_H
#define SCENE_H

#include "ISceneNode.h"

#include <memory>
#include <vector>
using namespace std;

#include <glm\glm.hpp>

class Model;
class GraphicsEngine;

class Scene : public ISceneNode
{
private:
	vector<shared_ptr<ISceneNode>> nodes;
	GraphicsEngine* graphicsEngine;
public:
	Scene(GraphicsEngine *graphicsEngine);
	Scene(Scene&) = delete;
	void operator=(Scene&) = delete;
	void drawFull() const;
	void drawShadows() const;
	void addNode(shared_ptr<ISceneNode> sceneNode);
	virtual bool castsShadows() { return true; }
};

#endif
