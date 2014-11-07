// Name:
// Scene.cpp
// Description:
// Implementation file for Scene class.
// Notes:
// OS-Unaware
// See Scene.h for class usage details

#include "CustomMemory.h"

#include "Scene.h"

#include <list>
using namespace std;

Scene::Scene(GraphicsEngine *graphicsEngine)
{

}

void Scene::drawFull() const
{
	//Will eventually hold light data
	list<shared_ptr<ISceneNode>> lights;

	//Draw each node in turn
	for (vector<shared_ptr<ISceneNode>>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		(*it)->drawFull();
	}
}

void Scene::drawShadows() const
{
	//Will eventually hold light data
	list<shared_ptr<ISceneNode>> lights;

	//Draw each node in turn
	for (vector<shared_ptr<ISceneNode>>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		(*it)->drawShadows();
	}
}

void Scene::addNode(shared_ptr<ISceneNode> sceneNode)
{
	//Add the node to the list.
	nodes.push_back(sceneNode);
}
