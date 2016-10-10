#include "Physics.h"

Physics::Physics(std::vector<Sphere*> balls, std::vector<Block*> blocks, Room* &space, Paddle* &pad) {
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
	dynamicsWorld->setGravity(btVector3(-100, 0, 0));

	// The physics object(s) representing our ball(s)
	int len = balls.size();
	for(int i = 0; i < len; i++)
		ballShape.push_back(new btSphereShape(balls.at(i)->radius));

	// Create blocks
	len = blocks.size();
	for (int i = 0; i < len; i++)
		blockShape.push_back(new btBoxShape(btVector3(blocks.at(i)->width, blocks.at(i)->length, blocks.at(i)->height)));

	paddleShape = new btCapsuleShapeX(pad->dim.x, 4);

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
	blockMass = 0;
	paddleMass = 0;
	ballInertia = btVector3(0, 0, 0);
	blockInertia = btVector3(0, 0, 0);
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
	paddleMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pad->position.x, pad->position.y, pad->position.z)));
	paddleShape->calculateLocalInertia(paddleMass, paddleInertia);
	btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI(paddleMass, paddleMotionState, paddleShape, paddleInertia);
	paddleRigidBody = new btRigidBody(paddleRigidBodyCI);
	paddleRigidBody->setUserPointer(paddleRigidBody);
	userIndex[paddleRigidBody->getUserPointer()] = 8000;
	paddleRigidBody->setRestitution(0.0);
	paddleRigidBody->setFriction(1.0);
	paddleRigidBody->setRollingFriction(1.0);
	dynamicsWorld->addRigidBody(paddleRigidBody);

	len = blocks.size();
	for (int j = 0; j < len; j++) {
		blockShape.at(j)->calculateLocalInertia(blockMass, blockInertia);
		blockMotionState.push_back(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(blocks.at(j)->getPosition().x, 
																											blocks.at(j)->getPosition().y, 
																											blocks.at(j)->getPosition().z))));
		btRigidBody::btRigidBodyConstructionInfo blockRigidBodyCI(blockMass, blockMotionState.at(j), blockShape.at(j), blockInertia);
		blockRigidBody.push_back(new btRigidBody(blockRigidBodyCI));

		blockRigidBody.at(j)->setUserPointer(blockRigidBody.at(j));
		userIndex[blockRigidBody.at(j)->getUserPointer()] = j+1;
		paddleRigidBody->setUserPointer(paddleShape);
		blockRigidBody.at(j)->setFriction(1.0);
		blockRigidBody.at(j)->setRollingFriction(1.0);
		blockRigidBody.at(j)->setRestitution(.8);
		dynamicsWorld->addRigidBody(blockRigidBody.at(j));
	}
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 2048);
    bounce = Mix_LoadWAV("resources/bounce.wav");
}

int Physics::checkCollide(Paddle* &pad, std::vector<Block*> &blk) {
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
                Mix_PlayChannel(-1, bounce, 0);
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obA->getUserPointer()] == 1500)
					if (userIndex[obB->getUserPointer()] == 1500 || userIndex[obB->getUserPointer()] == 1000)
						ballRigidBody.at(0)->setLinearVelocity(btVector3(1000, -40, (rand() % 40) - 20));
				if (userIndex[obA->getUserPointer()] == 1000 || userIndex[obB->getUserPointer()] == 1000) {
					if((userIndex[obA->getUserPointer()] < MAX_BLOCKS) && (userIndex[obA->getUserPointer()] >= 0)) {
						int index = userIndex[obA->getUserPointer()];
						if ((index - 1) >= 0) {
							Block* hitBlock = blk.at(index - 1);
							if (hitBlock->type != Block::metal && hitBlock->damage == (hitBlock->durability - 1)) {
								userIndex[obA->getUserPointer()] = -1;
								dynamicsWorld->removeCollisionObject(blockRigidBody.at(index - 1));
							}
							return index;
						}
					}
					else if((userIndex[obB->getUserPointer()] < MAX_BLOCKS) && (userIndex[obB->getUserPointer()] >= 0)) {
						int index = userIndex[obB->getUserPointer()];
						if ((index - 1) >= 0) {
							Block* hitBlock = blk.at(index - 1);
							if (hitBlock->type != Block::metal && hitBlock->damage == (hitBlock->durability - 1)) {
								userIndex[obB->getUserPointer()] = -1;
								dynamicsWorld->removeCollisionObject(blockRigidBody.at(index - 1));
							}
							return index;
						}
					}
					else if (userIndex[obA->getUserPointer()] == 8000 || userIndex[obA->getUserPointer()] == 8000) {
						ballRigidBody.at(0)->setLinearVelocity(btVector3(200, pad->dV.y, pad->dV.z));
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
	dynamicsWorld->removeRigidBody(paddleRigidBody);
	paddleMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pad->position.x, pad->position.y+5, pad->position.z)));
	paddleShape->calculateLocalInertia(paddleMass, paddleInertia);
	btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI(paddleMass, paddleMotionState, paddleShape, paddleInertia);
	paddleRigidBody = new btRigidBody(paddleRigidBodyCI);
	paddleRigidBody->setRestitution(2.0);
	paddleRigidBody->setFriction(1.0);
	paddleRigidBody->setRollingFriction(1.0);
	dynamicsWorld->addRigidBody(paddleRigidBody);
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

	len = blockShape.size();
	for (int k = 0; k < len; k++) {
		delete blockShape.at(k);
		delete blockMotionState.at(k);
		delete blockRigidBody.at(k);
	}

//	delete ballInertia;
//	delete blockInertia;
}
