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
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
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
	balls.push_back(new Sphere(mSceneMgr));
	blocks.push_back(new Block(mSceneMgr, Block::wood, 1));
	engine = new Physics(balls, blocks, emptyRoom);

	/*
	srand(time(NULL));
	for (int i = 0; i < NUM_SPHERE; i++) {
		ball[i] = new Sphere(10 + (rand() % 30), mSceneMgr);
		ball[i]->setPos(i * 60 + 10, 100, 0);
		ball[i]->setVel(rand() % 2, rand() % 2, rand() % 2);
	}
	*/
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
