#pragma once
#ifndef __Block_h_
#define __Block_h_
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

class Block
{

public:
	enum Type { paper, wood, stone, brick, metal };
	enum Powerup { none, spare, sticky, explosion, penetrate, multi, widen, slow, gun };

	int id;
	Type type;
	int durability;
	int points;
	int damage;
	Powerup powerup;

	Block();
	Block(Ogre::SceneManager *newManager, Type t, int num);

	//void setPos(int x, int y, int z);
	//Ogre::Vector3 getPos();
	void destroy();
	//void collision();
	int getID();
private:
	Ogre::SceneManager *blockManager;
	Ogre::SceneNode* blockNode;
	Ogre::Entity* blockEntity;
	void buildBlock();
};

#endif // #ifndef __Block_h_
