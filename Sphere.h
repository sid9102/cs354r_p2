#pragma once
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <time.h>

#ifndef __Sphere_h_
#define __Sphere_h_

class Sphere
{
public:
	int radius;
	Ogre::Vector3 last;
	Ogre::Vector3 v;
	Sphere();
	Sphere(Ogre::SceneManager *newManager);
	Sphere(int r, Ogre::SceneManager *newManager);
	void update();
	void setVel(int x, int y, int z);
	void setPos(int x, int y, int z);
	void setRot(double x, double y, double z, double w);
	Ogre::Vector3 getPos();
	Ogre::Quaternion getRot();
private:
	Ogre::SceneManager *sphereManager;
	void buildSphere();
	Ogre::SceneNode* ballNode;
	Ogre::Entity* ballEntity;
};

#endif // #ifndef __Sphere_h_
