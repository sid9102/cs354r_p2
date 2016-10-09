#pragma once
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

#ifndef __Paddle_h_
#define __Paddle_h_

class Paddle
{
public:
	Ogre::Vector3 position;
	Ogre::Vector3 lPosition;
	Ogre::Vector3 dV;
	Ogre::Vector3 dim;
	Paddle(Ogre::SceneManager *newManager);
	Paddle(Ogre::SceneManager *newManager, Ogre::Vector3 nPos);
	void setPos(int x, int y, int z);
	void setDim(int w, int l, int h);
private:
	void buildPaddle();
	Ogre::SceneManager *paddleManager;
	Ogre::SceneNode* paddleNode;
	Ogre::Entity* paddleEntity;
};

#endif // #ifndef __Sphere_h_
