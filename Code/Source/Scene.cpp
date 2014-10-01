// Name:
// Scene.cpp
// Description:
// Implementation file for Scene class.
// Notes:
// OS-Unaware
// See Scene.h for class usage details

#include "CustomMemory.h"

#include "Scene.h"

Scene::Scene(GraphicsEngine *graphicsEngine)
{

}

void Scene::draw()
{
	for (vector<shared_ptr<ISceneNode>>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		(*it)->draw();
	}
}

void Scene::addNode(shared_ptr<ISceneNode> sceneNode)
{
	nodes.push_back(sceneNode);
}
