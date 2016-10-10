
#include "Block.h"

Block::Block() {
	id=1;
	type = paper;
	durability = 1;
	points = 1 ;
	damage = 0;
	powerup = none;
	buildBlock();
}

Block::Block(Ogre::SceneManager *newManager, Type t, int num) {
	id = num;
	type = t;
	switch(type)
	{
		case(paper):
		{
			durability = 1;
			points = 1;
			break;
		}
		case(wood):
		{
			durability = 2;
			points = 2;
			break;
		}
		case(stone):
		{
			durability = 3;
			points = 3;
			break;
		}
		case(brick):
		{
			durability = 4;
			points = 4;
			break;
		}
		case(metal):
		{
			durability = 1500;
			points = 0;
			break;
		}
		default:
		{
			durability = 1;
			points = 1;
			break;
		}
	}

	damage = 0;
	int random = rand() % 8;
	switch(random)
	{
		case(0):
		{
			powerup = spare;
			break;
		}
		case(1):
		{
			powerup = sticky;
			break;
		}
		case(2):
		{
			powerup = explosion;
			break;
		}
		case(3):
		{
			powerup = penetrate;
			break;
		}
		case(4):
		{
			powerup = multi;
			break;
		}
		case(5):
		{
			powerup = widen;
			break;
		}
		case(6):
		{
			powerup = slow;
			break;
		}
		case(7):
		{
			powerup = gun;
			break;
		}
		default:
		{
			powerup = none;
			break;
		}
	}
	blockManager = newManager;
	buildBlock();
}

/*void Block::setPos(int x, int y, int z) {
	blockNode->setPosition(x, y, z);
}

Ogre::Vector3 Block::getPos() {
	return Ogre::Vector3(blockNode->getPosition().x, blockNode->getPosition().y, blockNode->getPosition().z);
}*/

void Block::destroy()
{
	damage = damage + 1;
	if(damage>=durability)
	{
		blockEntity->setVisible(false);	
		switch(powerup)
		{
			case(spare):
			{
				//addLife();	 
				break;
			}
			case(sticky):
			{
				//stick();
				break;
			}
			case(explosion):
			{
				break;
			}
			case(penetrate):
			{
				break;
			}
			case(multi):
			{
				break;
			}
			case(widen):
			{
				break;
			}
			case(slow):
			{
				break;
			}
			case(gun):
			{
				break;
			}
			default:
			{
				break;
			}
		}
	}
}


int Block::getID()
{
	return id;
}


void Block::buildBlock() {
	blockNode = blockManager->getRootSceneNode()->createChildSceneNode();
	switch(type)
	{
		case(paper):
		{
			blockEntity = blockManager->createEntity("paperBlock"+getID(), "Paper.mesh");
			break;
		}
		case(wood):
		{
			blockEntity = blockManager->createEntity("woodBlock"+getID(), "Wood.mesh");
			break;
		}
		case(stone):
		{
			blockEntity = blockManager->createEntity("stoneBlock"+getID(), "Stone.mesh");
			break;
		}
		case(brick):
		{
			blockEntity = blockManager->createEntity("brickBlock"+getID(), "Brick.mesh");
			break;
		}
		case(metal):
		{
			blockEntity = blockManager->createEntity("metalBlock"+getID(), "Metal.mesh");
			break;
		}
	}
	

	blockNode->setPosition(blockNode->getPosition().x, blockNode->getPosition().y, blockNode->getPosition().z);
	blockEntity->setCastShadows(true);
	blockNode->attachObject(blockEntity);
}

void Block::setPosition(int x, int y, int z) {
	blockNode->setPosition(Ogre::Vector3(x, y, z));
}
void Block::setSize(double x, double y, double z) {
	width = x;
	length = y;
	height = z;
	blockNode->setScale(Ogre::Vector3((width / 250.0) * 100.0, (length / 100.0) * 100.0, (height / 100.0) * 100.0));
}
Ogre::Vector3 Block::getPosition() {
	return blockNode->getPosition();
}
