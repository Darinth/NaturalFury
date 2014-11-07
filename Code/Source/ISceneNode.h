// Name:
// ISceneNode.h
// Description:
// Header file for ISceneNode.h
// ISceneNode is a virtual class used as the base things that are drawn into a scene.
// Notes:
// OS-Unaware

#ifndef I_SCENE_NODE_H
#define I_SCENE_NODE_H

class LightData;

class ISceneNode
{
public:
	//Draw the scene node
	virtual void drawFull() const = 0;
	//Draw elements of the scene node that should cast shadows
	virtual void drawShadows() const = 0;
	//Should this scene node cast shadows?
	virtual bool castsShadows() = 0;
};

#endif
