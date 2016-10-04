#include "Physics.h"

Physics::Physics(std::vector<Sphere*> balls, std::vector<Block*> blocks, Room* &space) {
	// Helps eliminate pairs of object that should not collide
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// Cuases objects to interact properly: crunches all the important stuff; gravity, supplied forces, collisions, etc.
	// TODO (maybe): Can implement multi-threading here for high performance simulation (do we really need this though)
	solver = new btSequentialImpulseConstraintSolver;

	// The area where all the physics take place
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	//Gravity set here, we need very light gravity (towards the paddle?)
	dynamicsWorld->setGravity(btVector3(-10, 0, 0));

	// The physics object representing our ball
	int len = balls.size();
	for(int i = 0; i < len; i++)
		ballShape.push_back(new btSphereShape(balls.at(i)->radius));

	// Create blocks
	len = blocks.size();
	for (int i = 0; i < len; i++)
		blockShape.push_back(new btBoxShape(btVector3(250, 100, 100)));

	// dimensions of ground here :D
	groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	wallShape.push_back(new btStaticPlaneShape(btVector3(1, 0, 0), -space->xOffset));
	wallShape.push_back(new btStaticPlaneShape(btVector3(-1, 0, 0), -space->xOffset));
	wallShape.push_back(new btStaticPlaneShape(btVector3(0, 0, 1), -space->yOffset));
	wallShape.push_back(new btStaticPlaneShape(btVector3(0, 0, -1), -space->yOffset));
	ceilShape = new btStaticPlaneShape(btVector3(0, -1, 0), -space->roomHeight);

	// instantiate the ground (we need to do this for the walls and the ceiling too in our case
	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	wallMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
	wallMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
	wallMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
	wallMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
	ceilMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	// Handle collision between the ball and the ground
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI(0, wallMotionState.at(0), wallShape.at(0), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI2(0, wallMotionState.at(1), wallShape.at(1), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI3(0, wallMotionState.at(2), wallShape.at(2), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI4(0, wallMotionState.at(3), wallShape.at(3), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo ceilRigidBodyCI(0, ceilMotionState, ceilShape, btVector3(0, 0, 0));

	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setFriction(1.0);
	groundRigidBody->setRollingFriction(1.0);
	groundRigidBody->setRestitution(1.0);
	groundRigidBody->setUserPointer(groundRigidBody);
	userIndex[groundRigidBody->getUserPointer()] == 5500;
	dynamicsWorld->addRigidBody(groundRigidBody);

	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI));
	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI2));
	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI3));
	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI4));
	for (int i = 0; i < 4; i++) {
		wallRigidBody.at(i)->setFriction(1.0);
		wallRigidBody.at(i)->setRollingFriction(1.0);
		wallRigidBody.at(i)->setRestitution(0);
		dynamicsWorld->addRigidBody(wallRigidBody.at(i));
	}
	wallRigidBody.at(1)->setRestitution(1.0);
	wallRigidBody.at(0)->setUserPointer(wallRigidBody.at(0));
	wallRigidBody.at(1)->setUserPointer(wallRigidBody.at(1));
	wallRigidBody.at(2)->setUserPointer(wallRigidBody.at(2));
	wallRigidBody.at(3)->setUserPointer(wallRigidBody.at(3));
	userIndex[wallRigidBody.at(0)->getUserPointer()] = 1500;
	userIndex[wallRigidBody.at(1)->getUserPointer()] = 2500;
	userIndex[wallRigidBody.at(2)->getUserPointer()] = 3500;
	userIndex[wallRigidBody.at(3)->getUserPointer()] = 4500;

	ceilRigidBody = new btRigidBody(ceilRigidBodyCI);
	ceilRigidBody->setFriction(1.0);
	ceilRigidBody->setRollingFriction(1.0);
	ceilRigidBody->setRestitution(1.0);
	dynamicsWorld->addRigidBody(ceilRigidBody);
	ceilRigidBody->setUserPointer(ceilRigidBody);
	userIndex[ceilRigidBody->getUserPointer()] = 6500;

	// Set mass and motion for the ball
	// NOTE: The btVector 3 here is the starting position for the ball
	ballMass = 1;
	blockMass = 0;
	ballInertia = btVector3(0, 0, 0);
	blockInertia = btVector3(0, 0, 0);

	len = balls.size();
	for (int j = 0; j < len; j++) {
		ballMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 400, 0))));

		ballShape.at(j)->calculateLocalInertia(ballMass, ballInertia);


		btRigidBody::btRigidBodyConstructionInfo physBallRigidBodyCI(ballMass, ballMotionState.at(j), ballShape.at(j), ballInertia);
		ballRigidBody.push_back(new btRigidBody(physBallRigidBodyCI));

		//set initial velocity of ball
		ballRigidBody.at(j)->setLinearVelocity(btVector3(0, 40, -20));
		ballRigidBody.at(j)->setRestitution(1.0);
		ballRigidBody.at(j)->setUserPointer(ballRigidBody.at(j));
		userIndex[ballRigidBody.at(j)->getUserPointer()] = 1000 + 1000 * (j);
		ballRigidBody.at(j)->setFriction(1.0);
		ballRigidBody.at(j)->setRollingFriction(1.0);
		dynamicsWorld->addRigidBody(ballRigidBody.at(j));
	}
	
	len = blocks.size();
	for (int j = 0; j < len; j++) {
		blockShape.at(j)->calculateLocalInertia(blockMass, blockInertia);
		blockMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
		btRigidBody::btRigidBodyConstructionInfo blockRigidBodyCI(blockMass, blockMotionState.at(j), blockShape.at(j), blockInertia);
		blockRigidBody.push_back(new btRigidBody(blockRigidBodyCI));

		blockRigidBody.at(j)->setUserPointer(blockRigidBody.at(j));
		userIndex[blockRigidBody.at(j)->getUserPointer()] = j;
		blockRigidBody.at(j)->setFriction(1.0);
		blockRigidBody.at(j)->setRollingFriction(1.0);
		blockRigidBody.at(j)->setRestitution(1.0);
		dynamicsWorld->addRigidBody(blockRigidBody.at(j));
	}
}

int Physics::checkCollide() {
	//Assume world->stepSimulation or world->performDiscreteCollisionDetection has been called
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		srand(time(0));

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if ((pt.getDistance() <= 0 + 0.0001) || (pt.getDistance() <= 0 - 0.0001))
			{
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obA->getUserPointer()] == 1500)
					if (userIndex[obB->getUserPointer()] == 1500 || userIndex[obB->getUserPointer()] == 1000)
						ballRigidBody.at(0)->setLinearVelocity(btVector3((rand() % 200) + 100, (rand() % 40) - 20, (rand() % 40) - 20));
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obB->getUserPointer()] == 1000) {
					/*if (userIndex[obA->getUserPointer()] < 100 || userIndex[obB->getUserPointer()] < 100) {
						if (userIndex[obA->getUserPointer()] < 100) {
							blockShape.at(userIndex[obA->getUserPointer()]) = NULL;
							blockRigidBody.at(userIndex[obA->getUserPointer()]) = NULL;
							return userIndex[obA->getUserPointer()];
						} else {
							blockShape.at(userIndex[obB->getUserPointer()]) = NULL;
							blockRigidBody.at(userIndex[obB->getUserPointer()]) = NULL;
							return userIndex[obB->getUserPointer()];
						}
					}*/
					if(userIndex[obA->getUserPointer()] < 100) {
						dynamicsWorld->removeCollisionObject(blockRigidBody.at(userIndex[obA->getUserPointer()]));
						return userIndex[obA->getUserPointer()];
					}
					else if(userIndex[obB->getUserPointer()] < 100) {
						dynamicsWorld->removeCollisionObject(blockRigidBody.at(userIndex[obB->getUserPointer()]));
						return userIndex[obB->getUserPointer()];
					}
				}
			}
		}
	}
	return -1;
}

void Physics::update() {
	dynamicsWorld->stepSimulation(1 / 60.f, 10);
}

Physics::~Physics() {
	// Clean up once the simulation ends
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}
