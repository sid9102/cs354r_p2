#include "Paddle.h"

Paddle::Paddle(Ogre::SceneManager *newManager) {
	paddleManager = newManager;
	dim = Ogre::Vector3(80, 12, 120);
	position = Ogre::Vector3(0, 0, 0);
	lPosition = Ogre::Vector3(0, 0, 0);
	dV = Ogre::Vector3(0, 0, 0);
	playerNum = 1;
	buildPaddle();
}
Paddle::Paddle(Ogre::SceneManager *newManager, Ogre::Vector3 nPos, int num) {
	paddleManager = newManager;
	position = nPos;
	lPosition = nPos;
	dV = Ogre::Vector3(0, 0, 0);
	dim = Ogre::Vector3(80, 12, 120);
	playerNum = num;
	buildPaddle();
}

void Paddle::setPos(int x, int y, int z) {
	position = Ogre::Vector3(x, y, z);
	paddleNode->setPosition(x, y, z);
}

void Paddle::buildPaddle() {
	paddleNode = paddleManager->getRootSceneNode()->createChildSceneNode("paddleNode" + playerNum);
	paddleEntity = paddleManager->createEntity("Paddle.mesh");
	paddleNode->attachObject(paddleEntity);
	paddleNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-90));
	paddleNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90));
	paddleNode->setPosition(position);
	paddleNode->setScale((dim.x / 200) * 100, (dim.y / 30) * 100, (dim.z / 300) * 100);
}

void Paddle::setDim(int w, int l, int h) {
	dim = Ogre::Vector3(w, l, h);
}

