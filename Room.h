#include "Sphere.h"
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreMeshManager.h>
#ifndef __Room_h_
#define __Room_h_

class Room
{
	public:
		int roomHeight;
		int roomLength;
		int roomWidth;
		int xOffset;
		int yOffset;
		int zOffset;
		Room();
		Room(Ogre::SceneManager *newManager);
		Room(int xyz, Ogre::SceneManager *newManager);
		Room(int x, int y, int z, Ogre::SceneManager *newManager);
		void checkCollide(Sphere* ball);
		void checkCollide(Sphere* ball, Sphere* ball2);
	private:
		Ogre::Entity* groundEntity;
		Ogre::Entity* wallEntity;
		Ogre::SceneNode* wallNode;
		Ogre::Entity* wallEntity2;
		Ogre::SceneNode* wallNode2;
		Ogre::Entity* wallEntity3;
		Ogre::SceneNode* wallNode3;
		Ogre::Entity* wallEntity4;
		Ogre::SceneNode* wallNode4;
		Ogre::Entity* roofEntity;
		Ogre::SceneNode* roofNode;
		Ogre::SceneManager *roomManager;
		void buildRoom();
};

#endif // #ifndef __Room_h_