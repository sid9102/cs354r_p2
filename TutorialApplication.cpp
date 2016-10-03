/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#include "TutorialApplication.h"
#define ROOM_DIM 1000

//---------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//---------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//---------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	// Set Scene properties
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);

	// Create a light and position it in the room
	//Ogre::Light* light = mSceneMgr->createLight("MainLight");
	//light->setPosition(20, 80, 50);

	// A spotlight
	Ogre::Light* spotlight = mSceneMgr->createLight("Spotlight");
	spotlight->setDiffuseColour(0, 0, 1.0);
	spotlight->setSpecularColour(0, 0, 1.0);
	spotlight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotlight->setDirection(-1, -1, 0);
	spotlight->setPosition(Ogre::Vector3(200, 200, 0));

	// A directional light
	Ogre::Light* directionalLight = mSceneMgr->createLight("DirectionalLight");
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	directionalLight->setDiffuseColour(Ogre::ColourValue(.4, 0, 0));
	directionalLight->setSpecularColour(Ogre::ColourValue(.4, 0, 0));
	directionalLight->setDirection(Ogre::Vector3(0, -1, 1));

	emptyRoom = new Room(1500, 500, 500, mSceneMgr);
	ball = new Sphere(mSceneMgr);

	/*
	srand(time(NULL));
	for (int i = 0; i < NUM_SPHERE; i++) {
		ball[i] = new Sphere(10 + (rand() % 30), mSceneMgr);
		ball[i]->setPos(i * 60 + 10, 100, 0);
		ball[i]->setVel(rand() % 2, rand() % 2, rand() % 2);
	}
	*/

	
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
	physBall = new btSphereShape(ball->radius);

	// dimensions of ground here :D
	groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	wallShape.push_back(new btStaticPlaneShape(btVector3(1, 0, 0), -750));
	wallShape.push_back(new btStaticPlaneShape(btVector3(-1, 0, 0), -750));
	wallShape.push_back(new btStaticPlaneShape(btVector3(0, 0, 1), -250));
	wallShape.push_back(new btStaticPlaneShape(btVector3(0, 0, -1), -250));
	ceilShape = new btStaticPlaneShape(btVector3(0, -1, 0), -500);

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
	wallRigidBody.at(0)->setUserIndex(1500);

	ceilRigidBody = new btRigidBody(ceilRigidBodyCI);
	ceilRigidBody->setFriction(1.0);
	ceilRigidBody->setRollingFriction(1.0);
	ceilRigidBody->setRestitution(1.0);
	dynamicsWorld->addRigidBody(ceilRigidBody);

	// Set mass and motion for the ball
	// NOTE: The btVector 3 here is the starting position for the ball
	fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 400, 0)));
	mass = 1;
	fallIntertia = btVector3(0, 0, 0);
	physBall->calculateLocalInertia(mass, fallIntertia);


	btRigidBody::btRigidBodyConstructionInfo physBallRigidBodyCI(mass, fallMotionState, physBall, fallIntertia);
	physBallRigidBody = new btRigidBody(physBallRigidBodyCI);

	//set initial velocity of ball
	physBallRigidBody->setLinearVelocity(btVector3(0, 40, -20));
	physBallRigidBody->setRestitution(1.0);
	physBallRigidBody->setUserIndex(1000);
	physBallRigidBody->setFriction(1.0);
	physBallRigidBody->setRollingFriction(1.0);
	dynamicsWorld->addRigidBody(physBallRigidBody);

	// Clean up once the simulation ends
	// delete dynamicsWorld;
	// delete solver;
	// delete dispatcher;
	// delete collisionConfiguration;
	// delete broadphase;
	
}
//---------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
