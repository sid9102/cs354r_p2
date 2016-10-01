#include "Room.h"
#include <SDL2/SDL.h>
Room::Room() {
	roomLength = roomWidth = roomHeight = 500;
	xOffset = yOffset = zOffset = 250;
	buildRoom();
}

Room::Room(Ogre::SceneManager *newManager) {
	roomLength = roomWidth = roomHeight = 500;
	xOffset = yOffset = zOffset = 250;
	roomManager = newManager;
	buildRoom();
}

Room::Room(int xyz, Ogre::SceneManager *newManager) {
	roomLength = xyz;
	roomWidth = xyz;
	roomHeight = xyz;
	xOffset = xyz >> 1;
	yOffset = xyz >> 1;
	zOffset = xyz >> 1;
	roomManager = newManager;
	buildRoom();
}

Room::Room(int x, int y, int z, Ogre::SceneManager *newManager) {
	roomWidth = x;
	roomLength = y;
	roomHeight = z;
	xOffset = roomWidth >> 1;
	yOffset = roomLength >> 1;
	zOffset = roomHeight >> 1;
	roomManager = newManager;
	buildRoom();
}

void Room::checkCollide(Sphere* ball) {

	Ogre::Vector3 cPos = ball->getPos();
	Ogre::Vector3 vBound = Ogre::Vector3(cPos.x + ball->v.x, cPos.y + ball->v.y, cPos.z + ball->v.z);

	if (vBound.x > (xOffset-ball->radius)) {
		ball->setPos(xOffset-ball->radius, cPos.y, cPos.z);
		ball->v.x = -ball->v.x;
	}
	if (vBound.x < (-xOffset+ball->radius)) {
		ball->setPos(-xOffset+ball->radius, cPos.y, cPos.z);
		ball->v.x = -ball->v.x;
	}
	if (vBound.y >= (roomHeight-ball->radius)) {
		ball->setPos(cPos.x, roomHeight-ball->radius, cPos.z);
		ball->v.y = -ball->v.y;
	}
	if (vBound.y <= (0+ball->radius)) {
		ball->setPos(cPos.x, 0+ball->radius, cPos.z);
		ball->v.y = -ball->v.y;
	}
	if (vBound.z >= (zOffset-ball->radius)) {
		ball->setPos(cPos.x, cPos.y, zOffset - ball->radius);
		ball->v.z = -ball->v.z;
	}
	if (vBound.z <= (-zOffset+ball->radius)) {
		ball->setPos(cPos.x, cPos.y, -zOffset + ball->radius);
		ball->v.z = -ball->v.z;
	}
}

void Room::checkCollide(Sphere* ball, Sphere* ball2) {
	Ogre::Vector3 dist = ball->getPos() - ball2->getPos();
	Ogre::Vector3 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y;

	double m1, m2, x1, x2;

	Ogre::Vector3 v1temp, v1, v2, v1x, v2x, v1y, v2y;

	Ogre::Vector3 x = ball->getPos() - ball2->getPos();

	int sumRadii = ball->radius + ball2->radius;
	if (dist.length() < sumRadii) {
		x.normalise();
		v1 = ball->v;
		x1 = x.dotProduct(v1);
		v1x = x * x1;
		v1y = v1 - v1x;
		m1 = ball->radius;

		x = x*-1;
		v2 = ball2->v;
		x2 = x.dotProduct(v2);
		v2x = x * x2;
		v2y = v2 - v2x;
		m2 = ball2->radius;

		ball->v = Ogre::Vector3(v1x*(m1 - m2) / (m1 + m2) + v2x*(2 * m2) / (m1 + m2) + v1y);
		ball2->v = Ogre::Vector3(v1x*(2 * m1) / (m1 + m2) + v2x*(m2 - m1) / (m1 + m2) + v2y);
	}

}

void Room::buildRoom() {
	// Create the ground
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"ground",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, roomWidth, roomLength, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	groundEntity = roomManager->createEntity("ground");
	roomManager->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setCastShadows(false);
	groundEntity->setMaterialName("Custom/Slime");

	// Create wall 1
	Ogre::Plane plane2(Ogre::Vector3::UNIT_X, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"wall",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane2, roomHeight, roomLength, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	wallEntity = roomManager->createEntity("wall");

	wallNode = roomManager->getRootSceneNode()->createChildSceneNode();
	wallNode->setPosition(-xOffset, zOffset, wallNode->getPosition().y);
	wallNode->attachObject(wallEntity);

	wallEntity->setCastShadows(false);
	wallEntity->setMaterialName("Custom/PBrick");

	// Create wall 2
	Ogre::Plane plane3(Ogre::Vector3::NEGATIVE_UNIT_X, 0);

	Ogre::MeshManager::getSingleton().createPlane(
		"wall2",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane3, roomHeight, roomLength, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	 wallEntity2 = roomManager->createEntity("wall2");

	wallNode2 = roomManager->getRootSceneNode()->createChildSceneNode();
	wallNode2->setPosition(xOffset, zOffset, wallNode2->getPosition().y);
	wallNode2->attachObject(wallEntity2);

	wallEntity2->setCastShadows(false);
	wallEntity2->setMaterialName("Custom/PBrick");

	// Create wall 3
	Ogre::Plane plane4(Ogre::Vector3::UNIT_Z, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"wall3",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane4, roomHeight, roomWidth, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_X);
	wallEntity3 = roomManager->createEntity("wall3");

	wallNode3 = roomManager->getRootSceneNode()->createChildSceneNode();
	wallNode3->setPosition(wallNode3->getPosition().x, zOffset, -yOffset);
	wallNode3->attachObject(wallEntity3);

	wallEntity3->setCastShadows(false);
	wallEntity3->setMaterialName("Custom/PBrick");

	// Create wall 4
	Ogre::Plane plane5(Ogre::Vector3::NEGATIVE_UNIT_Z, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"wall4",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane5, roomHeight, roomWidth, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_X);
	wallEntity4 = roomManager->createEntity("wall4");

	Ogre::SceneNode* wallNode4 = roomManager->getRootSceneNode()->createChildSceneNode();
	wallNode4->setPosition(wallNode4->getPosition().x, zOffset, yOffset);
	wallNode4->attachObject(wallEntity4);

	wallEntity4->setCastShadows(false);
	wallEntity4->setMaterialName("Custom/PBrick");


	// Create roof
	Ogre::Plane plane6(Ogre::Vector3::NEGATIVE_UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"roof",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane6, roomLength, roomWidth, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_X);
	roofEntity = roomManager->createEntity("roof");

	roofNode = roomManager->getRootSceneNode()->createChildSceneNode();
	roofNode->setPosition(roofNode->getPosition().x, roomHeight, roofNode->getPosition().z);
	roofNode->attachObject(roofEntity);

	roofEntity->setCastShadows(false);
	roofEntity->setMaterialName("Custom/PBrick");
}