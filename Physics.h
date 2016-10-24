#pragma once
#ifndef __Physics_h_
#define __Physics_h_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <vector>
#include <map>
#include "Sphere.h"
#include "Room.h"
#include "Block.h"
#include "Paddle.h"
#include <SDL.h>
#include <SDL_mixer.h>

#define MAX_BLOCKS 999
class Physics
{
public:
	// Public Members
	std::map<void*, int>		userIndex;		// Map holds the index of all objects
	std::vector<btRigidBody*>	ballRigidBody;	// Holds the pointer to the physics objects for ball
	std::vector<btRigidBody*>	blockRigidBody; // Holds the pointer to the physics objects for block
	btRigidBody*			paddleRigidBody1;// Holds the pointer to the hit area for the paddle1
	btRigidBody*			paddleRigidBody2;// Holds the pointer to the hit area for the paddle2

	// Constructor and deconstructor declared here
	Physics(std::vector<Sphere*>balls, Room* &space, Paddle* &pad1, Paddle* &pad2);
	~Physics();

	int checkCollide(Paddle* &pad1, Paddle* &pad2); // Check for collisions between specific objects
	void update(double tStep, double rate);					  // Update the simulation (simulation step)
	void updatePaddle(Paddle* &pad);						  // Update position of the paddle's hit area to match the paddle graphic

	// Sounds
	Mix_Chunk* bounce;
	bool soundOn = true;
private:
	// Basic Physics Variables
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	// Dynamic Objects (blocks, balls, powerups, etc.)
	std::vector<btCollisionShape*> ballShape; // mass = 1
	btCollisionShape*			   paddleShape; // mass = 0;

	// Static Objects (walls, ceiling, floor)
	btCollisionShape* groundShape;
	std::vector<btCollisionShape*> wallShape;
	btCollisionShape* ceilShape;

	// Motion states for objects
	std::vector<btDefaultMotionState*> ballMotionState;
	btDefaultMotionState* groundMotionState;
	std::vector<btDefaultMotionState*> wallMotionState;
	btDefaultMotionState* ceilMotionState;
	btDefaultMotionState* paddleMotionState1;
	btDefaultMotionState* paddleMotionState2;

	// Rigid bodies (the physical objects that interact with each other in the world)
	btRigidBody* groundRigidBody;
	std::vector<btRigidBody*> wallRigidBody;
	btRigidBody* ceilRigidBody;

	// SetUp Dynamic Objects
	btScalar ballMass;
	btScalar paddleMass;
	btVector3 ballInertia;
	btVector3 paddleInertia;
};
#endif // #ifndef __Physics_h_
