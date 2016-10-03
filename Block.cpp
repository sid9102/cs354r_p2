
#include "Block.h"

Block::Block() {
	t = paper;
	durability = 1;
	points = 1 ;
	damage = 0;
	powerup = none;
	buildBlock();
}

Block::Block(Ogre::SceneManager *newManager, Type t, int num) {
	Type type = t;
	switch(type)
	{
		case(paper):
		{
			durability = 1;
			points = 1 ;
		}
		case(wood):
		{
			durability = 2;
			points = 2;
		}
		case(stone):
		{
			durability = 3;
			points = 3;
		}
		case(brick):
		{
			durability = 4;
			points = 4;
		}
		case(metal):
		{
			durability = 1000;
			points = 0;
		}
	}
	damage = 0;
	powerup = none;
	blockManager = newManager;
	buildBlock();
}

void Block::setPos(int x, int y, int z) {
	blockNode->setPosition(x, y, z);
}

Ogre::Vector3 Block::getPos() {
	return Ogre::Vector3(blockNode->getPosition().x, blockNode->getPosition().y, blockNode->getPosition().z);
}

void Block::destroy()
{
}

void Block::collision()
{
	damage++;
}

int Block::getID()
{
	return id;
}


void Block::buildBlock() {
	blockNode = blockManager->getRootSceneNode()->createChildSceneNode();
	switch(t)
	{
		case(paper):
		{
			blockEntity = blockManager->createEntity("paperBlock"+getID(), "paper.mesh");
		}
		case(wood):
		{
			blockEntity = blockManager->createEntity("woodBlock"+getID(), "wood.mesh");
			blockNode->attachObject(blockEntity);
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
