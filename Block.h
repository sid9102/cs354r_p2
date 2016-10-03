#ifndef __Block_h_
#define __Block_h_
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
class Block
{

public:
	Block();
	enum Type { paper, wood, stone, brick, metal };
	enum Powerup { none, spare, sticky, explosion, penetrate, multi, widen, slow, gun };
	void setPos(int x, int y, int z);
	Ogre::Vector3 getPos();
	Block(Ogre::SceneManager *newManager, Type t, int num);
	void destroy();
	void collision();
	int getID();
private:
	int id;
	Type t;
	int durability;
	int points;
	int damage;
	Powerup powerup;
	Ogre::SceneManager *blockManager;
	Ogre::SceneNode* blockNode;
	Ogre::Entity* blockEntity;
	void buildBlock();
};

#endif // #ifndef __Block_h_
