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
	// Public Members
	std::map<void*, int>		userIndex;		// Map holds the index of all objects
	std::vector<btRigidBody*>	ballRigidBody;	// Holds the pointer to the physics objects for ball
	std::vector<btRigidBody*>	blockRigidBody; // Holds the pointer to the physics objects for block

	// Constructor and deconstructor declared here
	Physics(std::vector<Sphere*>balls, std::vector<Block*> blocks, Room* &space);
	~Physics();

	int checkCollide();							// Check for collisions between specific objects
	void update(double tStep, double rate);		// Update the simulation (simulation step)
private:
	// Basic Physics Variables
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	// Dynamic Objects (blocks, balls, powerups, etc.)
	std::vector<btCollisionShape*> ballShape; // mass = 1
	std::vector<btCollisionShape*> blockShape; // mass = 0;

	// Static Objects (walls, ceiling, floor)
	btCollisionShape* groundShape;
	std::vector<btCollisionShape*> wallShape;
	btCollisionShape* ceilShape;

	// Motion states for objects
	std::vector<btDefaultMotionState*> ballMotionState;
	std::vector<btDefaultMotionState*> blockMotionState;
	btDefaultMotionState* groundMotionState;
	std::vector<btDefaultMotionState*> wallMotionState;
	btDefaultMotionState* ceilMotionState;

	// Rigid bodies (the physical objects that interact with each other in the world)
	btRigidBody* groundRigidBody;
	std::vector<btRigidBody*> wallRigidBody;
	btRigidBody* ceilRigidBody;

	// SetUp Dynamic Objects
	btScalar ballMass;
	btScalar blockMass;
	btVector3 ballInertia;
	btVector3 blockInertia;
};
#endif // #ifndef __Physics_h_