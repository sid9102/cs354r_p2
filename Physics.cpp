#include "Physics.h"

Physics::Physics(std::vector<Sphere*> balls, Room* &space, Paddle* &pad1, Paddle* &pad2) {
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
	dynamicsWorld->setGravity(btVector3(-300, 0, 0));

	// The physics object(s) representing our ball(s)
	int len = balls.size();
	for(int i = 0; i < len; i++)
		ballShape.push_back(new btSphereShape(balls.at(i)->radius));

	paddleShape = new btCapsuleShapeX(pad1->dim.x, 4);

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

	// Handle collision between the ball and the ground, walls, and ceiling
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI(0, wallMotionState.at(0), wallShape.at(0), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI2(0, wallMotionState.at(1), wallShape.at(1), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI3(0, wallMotionState.at(2), wallShape.at(2), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI4(0, wallMotionState.at(3), wallShape.at(3), btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo ceilRigidBodyCI(0, ceilMotionState, ceilShape, btVector3(0, 0, 0));

	// Setu up the properties of the ground
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setFriction(1.0);
	groundRigidBody->setRollingFriction(1.0);
	groundRigidBody->setRestitution(1.0);
	groundRigidBody->setUserPointer(groundRigidBody);
	userIndex[groundRigidBody->getUserPointer()] == 5500;
	dynamicsWorld->addRigidBody(groundRigidBody);

	// Set up the properties of the walls
	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI));
	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI2));
	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI3));
	wallRigidBody.push_back(new btRigidBody(wallRigidBodyCI4));
	for (int i = 0; i < 4; i++) {
		wallRigidBody.at(i)->setFriction(1.0);
		wallRigidBody.at(i)->setRollingFriction(1.0);
		wallRigidBody.at(i)->setRestitution(0.5);
		dynamicsWorld->addRigidBody(wallRigidBody.at(i));
	}
	wallRigidBody.at(0)->setUserPointer(wallRigidBody.at(0));
	wallRigidBody.at(1)->setUserPointer(wallRigidBody.at(1));
	wallRigidBody.at(2)->setUserPointer(wallRigidBody.at(2));
	wallRigidBody.at(3)->setUserPointer(wallRigidBody.at(3));
	userIndex[wallRigidBody.at(0)->getUserPointer()] = 1500;
	userIndex[wallRigidBody.at(1)->getUserPointer()] = 2500;
	userIndex[wallRigidBody.at(2)->getUserPointer()] = 3500;
	userIndex[wallRigidBody.at(3)->getUserPointer()] = 4500;

	// Set up the properties of the ceiling
	ceilRigidBody = new btRigidBody(ceilRigidBodyCI);
	ceilRigidBody->setFriction(1.0);
	ceilRigidBody->setRollingFriction(1.0);
	ceilRigidBody->setRestitution(0.5);
	dynamicsWorld->addRigidBody(ceilRigidBody);
	ceilRigidBody->setUserPointer(ceilRigidBody);
	userIndex[ceilRigidBody->getUserPointer()] = 6500;

	// Set mass and motion for the ball
	// NOTE: The btVector 3 here is the starting position for the ball
	ballMass = 1;
	paddleMass = 0;
	ballInertia = btVector3(0, 0, 0);
	paddleInertia = btVector3(0, 0, 0);

	len = balls.size();
	for (int j = 0; j < len; j++) {
		ballMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 400, 0))));
		ballShape.at(j)->calculateLocalInertia(ballMass, ballInertia);
		btRigidBody::btRigidBodyConstructionInfo physBallRigidBodyCI(ballMass, ballMotionState.at(j), ballShape.at(j), ballInertia);
		ballRigidBody.push_back(new btRigidBody(physBallRigidBodyCI));

		//set initial velocity of ball
		ballRigidBody.at(j)->setLinearVelocity(btVector3(0, 40, -1000));
		ballRigidBody.at(j)->setRestitution(1.0);
		ballRigidBody.at(j)->setUserPointer(ballRigidBody.at(j));
		userIndex[ballRigidBody.at(j)->getUserPointer()] = 1000 + 1000 * (j);
		ballRigidBody.at(j)->setFriction(1.0);
		ballRigidBody.at(j)->setRollingFriction(1.0);
		dynamicsWorld->addRigidBody(ballRigidBody.at(j));
	}
	
	//Set properties for Paddle
	//Paddle 1
	paddleMotionState1 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pad1->position.x, pad1->position.y, pad1->position.z)));
	paddleShape->calculateLocalInertia(paddleMass, paddleInertia);
	btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI1(paddleMass, paddleMotionState1, paddleShape, paddleInertia);
	paddleRigidBody1 = new btRigidBody(paddleRigidBodyCI1);
	paddleRigidBody1->setUserPointer(paddleRigidBody1);
	userIndex[paddleRigidBody1->getUserPointer()] = 8000;
	paddleRigidBody1->setRestitution(0.0);
	paddleRigidBody1->setFriction(1.0);
	paddleRigidBody1->setRollingFriction(1.0);
	dynamicsWorld->addRigidBody(paddleRigidBody1);

	//Paddle 2
	paddleMotionState2 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pad2->position.x, pad2->position.y, pad2->position.z)));
	btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI2(paddleMass, paddleMotionState2, paddleShape, paddleInertia);
	paddleRigidBody2 = new btRigidBody(paddleRigidBodyCI2);
	paddleRigidBody2->setUserPointer(paddleRigidBody2);
	userIndex[paddleRigidBody2->getUserPointer()] = 16000;
	paddleRigidBody2->setRestitution(0.0);
	paddleRigidBody2->setFriction(1.0);
	paddleRigidBody2->setRollingFriction(1.0);
	dynamicsWorld->addRigidBody(paddleRigidBody2);

    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 2048);
    bounce = Mix_LoadWAV("resources/bounce.wav");
}

int Physics::checkCollide(Paddle* &pad1, Paddle* &pad2) {
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
			if (pt.getDistance() <= 0)
			{
				if(soundOn) { Mix_PlayChannel(-1, bounce, 0); }
                		//Frontwall
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obA->getUserPointer()] == 1500)
					if (userIndex[obB->getUserPointer()] == 1500 || userIndex[obB->getUserPointer()] == 1000) {
						//ballRigidBody.at(0)->setLinearVelocity(btVector3(1000, -40, (rand() % 40) - 20));
						dynamicsWorld->setGravity(btVector3(300, 0, 0));
						return -5;
					}
				//Backwall
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obA->getUserPointer()] == 2500)
					if (userIndex[obB->getUserPointer()] == 2500 || userIndex[obB->getUserPointer()] == 1000) {
						//ballRigidBody.at(0)->setLinearVelocity(btVector3(-1000, -40, (rand() % 40) - 20));
						dynamicsWorld->setGravity(btVector3(-300, 0, 0));
						return -10;
					}
				//p1 paddle + ball
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obA->getUserPointer()] == 8000) {
					if (userIndex[obB->getUserPointer()] == 8000 || userIndex[obB->getUserPointer()] == 1000) {
						//ballRigidBody.at(0)->setLinearVelocity(btVector3(200, pad1->dV.y, pad1->dV.z));
						dynamicsWorld->setGravity(btVector3(300, 0, 0));
					}
				}
				//p2 paddle + ball
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obA->getUserPointer()] == 16000) {
					if (userIndex[obB->getUserPointer()] == 16000 || userIndex[obB->getUserPointer()] == 1000) {
						//ballRigidBody.at(0)->setLinearVelocity(btVector3(-200, pad2->dV.y, pad2->dV.z));
						dynamicsWorld->setGravity(btVector3(-300, 0, 0));
					}
				}
			}
		}
	}
	return -1;
}

void Physics::update(double tStep, double rate) {
	dynamicsWorld->stepSimulation(tStep, rate);
}

void Physics::updatePaddle(Paddle* &pad) {
	if(pad->playerNum == 1) {
		dynamicsWorld->removeRigidBody(paddleRigidBody1);
		paddleMotionState1 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pad->position.x, pad->position.y+5, pad->position.z)));
		paddleShape->calculateLocalInertia(paddleMass, paddleInertia);
		btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI1(paddleMass, paddleMotionState1, paddleShape, paddleInertia);
		paddleRigidBody1 = new btRigidBody(paddleRigidBodyCI1);
		paddleRigidBody1->setRestitution(2.0);
		paddleRigidBody1->setFriction(1.0);
		paddleRigidBody1->setRollingFriction(1.0);
		userIndex[paddleRigidBody1->getUserPointer()] = 8000;
		dynamicsWorld->addRigidBody(paddleRigidBody1);
	}
	else if(pad->playerNum == 2) {
		dynamicsWorld->removeRigidBody(paddleRigidBody2);
		paddleMotionState2 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pad->position.x, pad->position.y+5, pad->position.z)));
		paddleShape->calculateLocalInertia(paddleMass, paddleInertia);
		btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI2(paddleMass, paddleMotionState2, paddleShape, paddleInertia);
		paddleRigidBody2 = new btRigidBody(paddleRigidBodyCI2);
		paddleRigidBody2->setRestitution(2.0);
		paddleRigidBody2->setFriction(1.0);
		paddleRigidBody1->setRollingFriction(1.0);
		userIndex[paddleRigidBody2->getUserPointer()] = 16000;
		dynamicsWorld->addRigidBody(paddleRigidBody2);
	}
}
Physics::~Physics() {
	int len;
	delete broadphase;
	
	for (std::map<void*, int>::iterator i; i != userIndex.cend(); i++) {
		userIndex.erase(i);
	}
	
	delete collisionConfiguration;
	delete dispatcher;
	delete solver;
	delete dynamicsWorld;
	len = ballShape.size();
	for (int i = 0; i < len; i++) {
		delete ballShape.at(i);
		delete ballMotionState.at(i);
		delete ballRigidBody.at(i);
	}

	delete groundShape;
	delete groundMotionState;
	delete groundRigidBody;

	delete ceilShape;
	delete ceilMotionState;
	delete ceilRigidBody;

	len = wallShape.size();
	for (int j = 0; j < len; j++) {
		delete wallShape.at(j);
		delete wallMotionState.at(j);
		delete wallRigidBody.at(j);
	}


//	delete ballInertia;
}
