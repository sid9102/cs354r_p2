#include "Sphere.h"

Sphere::Sphere() {
	v = (0, 0, 0);
	radius = 50;
	buildSphere();
}

Sphere::Sphere(Ogre::SceneManager *newManager) {
	v = (0, 0, 0);
	radius = 50;
	sphereManager = newManager;
	buildSphere();
}

Sphere::Sphere(int r, Ogre::SceneManager *newManager) {
	v = (0, 0, 0);
	radius = r;
	sphereManager = newManager;
	buildSphere();
}
void Sphere::update() {
	ballNode->setPosition(ballNode->getPosition().x + v.x, 
						  ballNode->getPosition().y + v.y,
						  ballNode->getPosition().z + v.z);
	
}
void Sphere::setVel(int x, int y, int z) {
	v.x = x;
	v.y = y;
	v.z = z;
}
void Sphere::setPos(int x, int y, int z) {
	ballNode->setPosition(x, y, z);
}
Ogre::Vector3 Sphere::getPos() {
	return Ogre::Vector3(ballNode->getPosition().x, ballNode->getPosition().y, ballNode->getPosition().z);
}

void Sphere::setRot(double x, double y, double z, double w) {
	ballNode->setOrientation(Ogre::Quaternion(-z, y, x, w));
}

Ogre::Quaternion Sphere::getRot() {
	return ballNode->getOrientation();
}

void Sphere::buildSphere() {
	static int numBall = 0;
	last = (0, 0, 0);
	ballNode = sphereManager->getRootSceneNode()->createChildSceneNode();
	ballEntity = sphereManager->createEntity("ball"+numBall, Ogre::SceneManager::PT_SPHERE);
	ballNode->setPosition(ballNode->getPosition().x, 100, ballNode->getPosition().z);
	ballNode->setScale(Ogre::Vector3((radius << 1) / 100.0, (radius << 1) / 100.0, (radius << 1) / 100.0));
	ballEntity->setMaterialName("Custom/CDemon");
	ballEntity->setCastShadows(true);
	ballNode->attachObject(ballEntity);
	numBall++;
} 