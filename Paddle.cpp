#include "Paddle.h"

Paddle::Paddle(Ogre::SceneManager *newManager) {
	paddleManager = newManager;
	dim = Ogre::Vector3(80, 12, 120);
	position = Ogre::Vector3(0, 0, 0);
	buildPaddle();
}
Paddle::Paddle(Ogre::SceneManager *newManager, Ogre::Vector3 nPos) {
	paddleManager = newManager;
	position = nPos;
	dim = Ogre::Vector3(80, 12, 120);
	buildPaddle();
}

void Paddle::setPos(int x, int y, int z) {
	position = Ogre::Vector3(x, y, z);
	paddleNode->setPosition(x, y, z);
}

void Paddle::buildPaddle() {
	paddleNode = paddleManager->getRootSceneNode()->createChildSceneNode("paddleNode");
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

