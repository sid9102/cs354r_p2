#pragma once
#ifndef __Physics_h_
#define __Physics_h_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <vector>
#include <map>
#include "Sphere.h"
#include "Room.h"
#include "Timer.h"
#include "Block.h"
class Physics
{
public:
	std::map<void*, int> userIndex;
	Physics(std::vector<Sphere*> balls, std::vector<Block*> blocks, Room* &space);
	~Physics();
	std::vector<btRigidBody*> ballRigidBody;
	std::vector<btRigidBody*> blockRigidBody;
	int checkCollide();
	void update();
private:
	// Timer for physics engine


	// Basic Physics Variables
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	// Dynamic Objects
	std::vector<btCollisionShape*> ballShape; // mass = 1
	std::vector<btCollisionShape*> blockShape; // mass = 0;

	// Static Objects
	btCollisionShape* groundShape;
	btDefaultMotionState* groundMotionState;
	btRigidBody* groundRigidBody;
	std::vector<btCollisionShape*> wallShape;
	std::vector<btDefaultMotionState*> wallMotionState;
	std::vector<btRigidBody*> wallRigidBody;
	btCollisionShape* ceilShape;
	btDefaultMotionState* ceilMotionState;
	btRigidBody* ceilRigidBody;
	std::vector<btDefaultMotionState*> ballMotionState;
	std::vector<btDefaultMotionState*> blockMotionState;

	// SetUp Ball Object
	btScalar ballMass;
	btScalar blockMass;
	btVector3 ballInertia;
	btVector3 blockInertia;
};
#endif // #ifndef __Physics_h_