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
	virtual void draw() = 0;
	virtual bool castsShadows() = 0;
	//virtual LightData getLightData();
};

#endif
