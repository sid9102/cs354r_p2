
#include "Block.h"

Block::Block() {
	type = paper;
	durability = 1;
	points = 1 ;
	damage = 0;
	powerup = 0;
	buildBlock();
}

Block::Block(Ogre::SceneManager *newManager, Type t, int num) {
	Type type = t;
	switch(type)
	{
		paper:
		{
			durability = 1;
			points = 1 ;
		}
		wood:
		{
			durability = 2;
			points = 2;
		}
		stone:
		{
			durability = 3;
			points = 3;
		}
		brick:
		{
			durability = 4;
			points = 4;
		}
		metal:
		{
			durability = 1000;
			points = 0;
		}
	}
	damage = 0;
	powerup = 0;
	blockManager = newManager;
	buildBlock();
}

void Block::setPos(int x, int y, int z) {
	blockNode->setPosition(x, y, z);
}

Ogre::Vector3 Block::getPos() {
	return Ogre::Vector3(blockNode->getPosition().x, blockNode->getPosition().y, blockNode->getPosition().z);
}

void destroy()
{
}

void collision()
{
	damage++;
}

int getID()
{
	return id;
}


void Sphere::buildBlock() {
	blockNode = blockManager->getRootSceneNode()->createChildSceneNode();
	switch(type)
	{
		case(paper):
		{
			blockEntity = blockManager->createEntity("paperBlock"+getID(), "paper.mesh");
		}
		case(wood):
		{
			blockEntity = blockManager->createEntity("woodBlock"+getID(), "wood.mesh");
		}
		case(stone):
		{
			blockEntity = blockManager->createEntity("stoneBlock"+getID(), "stone.mesh");
		}
		case(brick):
		{
			blockEntity = blockManager->createEntity("brickBlock"+getID(), "brick.mesh");
		}
		case(metal):
		{
			blockEntity = blockManager->createEntity("metalBlock"+getID(), "metal.mesh");
		}
	}
	blockNode->setPosition(blockNode->getPosition().x, blockNode->getPosition().y, blockNode->getPosition().z);
	
	blockEntity->setCastShadows(true);
	blockNode->attachObject(blockEntity);
} 
