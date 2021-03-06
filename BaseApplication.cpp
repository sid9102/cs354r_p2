/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
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

#include "BaseApplication.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <macUtils.h>
#endif
//---------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
        : mRoot(0),
          mCamera(0),
          mSceneMgr(0),
          mWindow(0),
          mResourcesCfg(Ogre::StringUtil::BLANK),
          mPluginsCfg(Ogre::StringUtil::BLANK),
          mTrayMgr(0),
          mCameraMan(0),
          mDetailsPanel(0),
          mCursorWasVisible(false),
          mShutDown(false),
          mInputManager(0),
          mMouse(0),
          mKeyboard(0),
          mOverlaySystem(0)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    m_ResourcePath = Ogre::macBundlePath() + "/Contents/Resources/";
#else
    m_ResourcePath = "";
#endif
}

//---------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;
    if (mOverlaySystem) delete mOverlaySystem;

    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

//---------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
    // Show the configuration dialog and initialise the system.
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg.
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot->initialise(true, "Paddlemania");
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    // Initialize the OverlaySystem (changed for Ogre 1.9)
    mOverlaySystem = new Ogre::OverlaySystem();
    mSceneMgr->addRenderQueueListener(mOverlaySystem);
}
//---------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setPosition(Ogre::Vector3(-740,250,0));
    mCamera->lookAt(Ogre::Vector3(1500,250,0));

    mCamera->setNearClipDistance(5);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // Create a default camera controller
}
//---------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem(pl);

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    // Set initial mouse clipping size
    windowResized(mWindow);

    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    mInputContext.mKeyboard = mKeyboard;
    mInputContext.mMouse = mMouse;
    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mInputContext, this);
    mTrayMgr->hideFrameStats();
    mTrayMgr->hideLogo();
    mTrayMgr->hideCursor();

    // Create a params panel for displaying sample details
    Ogre::StringVector items;
    if(multiplayer) {
        items.push_back("Player 1 Lives: ");
        items.push_back("Player 2 Lives: ");
    }
    else {
        items.push_back("Lives");
        items.push_back("Score");
    }
	/*
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("");
    items.push_back("Filtering");
    items.push_back("Poly Mode");
    items.push_back("FPS");
	*/
    mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
    //mDetailsPanel->setParamValue(9, "Bilinear");
    //mDetailsPanel->setParamValue(10, "Solid");
    //mDetailsPanel->hide();

    //mWinBox = mTrayMgr->createTextBox(OgreBites::TL_CENTER,"WinCap","Game Over", 100, 50);
    //mWinBox->hide();

    mRoot->addFrameListener(this);
}
//---------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//---------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//---------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app.
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location.
            if (!Ogre::StringUtil::startsWith(archName, "/", false)) // only adjust relative directories
                archName = Ogre::String(Ogre::macBundlePath() + "/" + archName);
#endif

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
        }
    }
}
//---------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{
}
//---------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//---------------------------------------------------------------------------
void BaseApplication::go(void)
{
#ifdef _DEBUG
    #ifndef OGRE_STATIC_LIB
    mResourcesCfg = m_ResourcePath + "resources_d.cfg";
    mPluginsCfg = m_ResourcePath + "plugins_d.cfg";
#else
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#endif
#else
#ifndef OGRE_STATIC_LIB
    mResourcesCfg = m_ResourcePath + "resources.cfg";
    mPluginsCfg = m_ResourcePath + "plugins.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif
#endif

    if (!setup())
        return;

    mRoot->startRendering();

    // Clean up
    destroyScene();
}
//---------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
    connectionOpened = false;
    mRoot = new Ogre::Root(mPluginsCfg);

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    mGUI = new SGUI();
    mGUI->setTitleScreenVisible(true);

    createFrameListener();

    // Initialize the SDL_Mixer
    SDL_Init(SDL_INIT_EVERYTHING);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 2048);
    bgm = Mix_LoadMUS("resources/bgm.wav");
    explosion = Mix_LoadWAV("resources/Explosion.wav");
    woosh = Mix_LoadWAV("resources/woosh.wav");
    metal_sound = Mix_LoadWAV("resources/Tone.wav");
    paper_sound = Mix_LoadWAV("resources/Paper_Rip.wav");
    brick_sound = Mix_LoadWAV("resources/Punch.wav");
    wood_sound = Mix_LoadWAV("resources/Neck_Snap.wav");
    stone_sound = Mix_LoadWAV("resources/Metal_Pot.wav");

    if(soundOn){
        Mix_PlayMusic(bgm, -1);
        Mix_Resume(-1);
    }
    else{
        Mix_Pause(-1);
        Mix_PauseMusic();
    }


    if(SDLNet_Init()==-1)
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);
    }

    lastUpdate = -1;
    return true;
};
//---------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (mWindow->isClosed())
        return false;

    if (mShutDown)
        return false;

    // Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    mTrayMgr->frameRenderingQueued(evt);
    mDetailsPanel->hide();

    if (!mTrayMgr->isDialogVisible()) {
        mCameraMan->frameRenderingQueued(evt);   // If dialog isn't up, then update the camera
        if (mDetailsPanel->isVisible())          // If details panel is visible, then update its contents
        {
            mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(p1lives)); // replace 0 w/score var
            if (multiplayer)
                mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(p2lives)); // replace 2 w/lives var
            else
                mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(score));
        }
    }
    if(multiplayer) {
        mGUI->updateP1Score(p1lives);
        mGUI->updateP2Score(p2lives);
    }
    if(mGUI->isStarted && sceneCreated) {
        if (p1lives <= 0 || p2lives <= 0 || score >= 640) {
            mGUI->setGOverScreenVisible(true);
            mGUI->setP1ScoreVisible(false);
            mGUI->setP2ScoreVisible(false);
            mGUI->setTimerVisible(false);
//            mDetailsPanel->hide();
            //mWinBox->show();
        }
        /*
        for (int i = 0; i < NUM_SPHERE; i++) {

            ball[i]->update();
            emptyRoom->checkCollide(ball[i]);
            for (int j = i + 1; j < NUM_SPHERE; j++) {
                emptyRoom->checkCollide(ball[i], ball[j]);
            }
        }
        */

        btTransform trans;
        int len = balls.size();
        int index;

        if (isServer) {
            for (int i = 0; i < len; i++) {
                engine->ballRigidBody.at(i)->getMotionState()->getWorldTransform(trans);
                balls.at(i)->setPos(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
                balls.at(i)->setRot(trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ(),
                                    trans.getRotation().getW());
            }
        }
        engine->update(evt.timeSinceLastFrame, 100);
        lastHit++;
        index = engine->checkCollide(paddle1, paddle2, blocks);

        if (index > 0) {
            score += blocks.at(index - 1)->destroy();
            if(!multiplayer) {
                mGUI->updateP1Score(score);
            }
            if (soundOn) {
                switch (blocks.at(index - 1)->type) {
                    case paper:
                        Mix_PlayChannel(-1, paper_sound, 0);
                        break;
                    case wood:
                        Mix_PlayChannel(-1, wood_sound, 0);
                        break;
                    case stone:
                        Mix_PlayChannel(-1, stone_sound, 0);
                        break;
                    case brick:
                        Mix_PlayChannel(-1, brick_sound, 0);
                        break;
                    case metal:
                        Mix_PlayChannel(-1, metal_sound, 0);
                        break;
                }
            }
        }
        // Can be right after p1lives, but won't 'initialize' properly then.
        if(!multiplayer) {
            mGUI->setTimer(p1lives);
        }
        if (isServer) {
            if (index == -5) {
                if (lastHit > 15) {
                    lastHit = 0;
                    p1lives--;
                }
            } else if (index == -10 && multiplayer) {
                if (lastHit > 15) {
                    lastHit = 0;
                    p2lives--;
                }
            }
        }

        /*/
        newTime = time(0);
        frameTime = newTime - currentTime;
        currentTime = time(0);

        while (frameTime >= 0) {

            frameTime -= dt;
        }*/
        //ball->update();
        //emptyRoom->checkCollide(ball);
        if (multiplayer) {
        updateClient();
        }
    }
    else if(mGUI->isStarted)
    {
        multiplayer = mGUI->multiStarted;
        isServer = mGUI->isServer;
        if (isServer){
            //Server
            mCamera->setPosition(Ogre::Vector3(-740,250,0));
        }
        else{
            //Client
            mCamera->setPosition(Ogre::Vector3(740,250,0));
        }
        // Look back along -Z
        if(isServer){
            mCamera->lookAt(Ogre::Vector3(1500,250,0));
        }
        else{
            mCamera->lookAt(Ogre::Vector3(-1500,250,0));
        }
        createScene();
        sceneCreated = true;
    }
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
{
    if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
    {
        mTrayMgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
    {
        if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
    }
    else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;
        /*
        switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
        {
        case 'B':
            newVal = "Trilinear";
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;
        case 'T':
            newVal = "Anisotropic";
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;
        case 'A':
            newVal = "None";
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;
        default:
            newVal = "Bilinear";
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        mDetailsPanel->setParamValue(9, newVal);
        */
    }
    else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mCamera->getPolygonMode())
        {
            case Ogre::PM_SOLID:
                newVal = "Wireframe";
                pm = Ogre::PM_WIREFRAME;
                break;
            case Ogre::PM_WIREFRAME:
                newVal = "Points";
                pm = Ogre::PM_POINTS;
                break;
            default:
                newVal = "Solid";
                pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode(pm);
        //mDetailsPanel->setParamValue(10, newVal);
    }
    else if(arg.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
    else if (arg.key == OIS::KC_P){
        if(soundOn){
            soundOn = false;
            engine->soundOn = false;
            Mix_HaltChannel(-1);
            Mix_PauseMusic();
        }
        else if(!soundOn){
            soundOn = true;
            engine->soundOn = true;
            Mix_Resume(-1);
            Mix_ResumeMusic();
        }
    }
//    else if (arg.key == OIS::KC_Z)

    /******************************************************************************
    ** CEGUI Handler for key events, do not delete!                             **
    ******************************************************************************/
    CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
    context.injectKeyDown((CEGUI::Key::Scan)arg.key);
    context.injectChar((CEGUI::Key::Scan)arg.text);
    // END CEGUI Handler
    mCameraMan->injectKeyDown(arg);
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::keyReleased(const OIS::KeyEvent &arg)
{
    mCameraMan->injectKeyUp(arg);
    /******************************************************************************
    ** CEGUI Handler for key events, do not delete!                             **
    ******************************************************************************/
    CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)arg.key);
    // END CEGUI KEY HANDLER
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::mouseMoved(const OIS::MouseEvent &arg)
{
    if(sceneCreated) {
        Ogre::Vector3 *paddleCoords;

        if (isServer) {
            //Server
            paddle1->lPosition = paddle1->position;
            paddleCoords = &paddle1->position;
        } else {
            //Client
            paddle2->lPosition = paddle2->position;
            paddleCoords = &paddle2->position;
        }
        if (mTrayMgr->injectMouseMove(arg)) return true;
//    mCameraMan->injectMouseMove(arg);
        float xDiff = arg.state.X.rel;
        float yDiff = arg.state.Y.rel;

        if (isServer) {
            //Server
            paddleCoords->z += xDiff;
            paddleCoords->y -= yDiff; // Was left out for some reason?
        } else {
            //Client
            paddleCoords->z -= xDiff;
            paddleCoords->y -= yDiff;
        }

        if (paddleCoords->z > 250) {
            paddleCoords->z = 250;
        } else if (paddleCoords->z < -250) {
            paddleCoords->z = -250;
        }

        if (paddleCoords->y > 500) {
            paddleCoords->y = 500;
        } else if (paddleCoords->y < 0) {
            paddleCoords->y = 0;
        }

        if (isServer) {
            //Server
            paddle1->setPos(paddleCoords->x, paddleCoords->y, paddleCoords->z);
            paddle1->dV = paddle1->lPosition - paddle1->position;
            engine->updatePaddle(paddle1);
            engine->updatePaddle(paddle2);
        } else {
            //Client
            paddle2->setPos(paddleCoords->x, paddleCoords->y, paddleCoords->z);
            paddle2->dV = paddle2->lPosition - paddle2->position;
            engine->updatePaddle(paddle2);
        }
        if ((xDiff > 25 || xDiff < -25 || yDiff > 25 || yDiff < -25) && soundOn) {
            Mix_PlayChannel(-1, woosh, 0);
        }
    }

    /******************************************************************************
     ** CEGUI Handler for mouse movement, do not delete!                         **
     ******************************************************************************/
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.getDefaultGUIContext().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
//    printf("xDiff: %f, yDiff: %f, paddleCoords->z:%f, paddleCoords->y:%f\n", xDiff, yDiff, paddleCoords->z, paddleCoords->y);
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);
    /******************************************************************************
    ** CEGUI Handler for mouse movement, do not delete!                         **
    ******************************************************************************/
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(mGUI->convertButton(id));
    // END OF CEGUI HANDLER
    return true;
}
//---------------------------------------------------------------------------
bool BaseApplication::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if (mTrayMgr->injectMouseUp(arg, id)) return true;
    mCameraMan->injectMouseUp(arg, id);
    /******************************************************************************
    ** CEGUI Handler for mouse movement, do not delete!                         **
    ******************************************************************************/
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(mGUI->convertButton(id));
    // END OF CEGUI HANDLER
    return true;
}
//---------------------------------------------------------------------------
// Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}
//---------------------------------------------------------------------------
// Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
    // Only close for window that created OIS (the main window in these demos)
    if(rw == mWindow)
    {
        if(mInputManager)
        {
            mInputManager->destroyInputObject(mMouse);
            mInputManager->destroyInputObject(mKeyboard);

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

clock_t getCurrentTime()
{
    return clock() / (CLOCKS_PER_SEC / 1000);
}

void BaseApplication::updateClient()
{
    /* Changes needed:
     * send information on the rotation of the sphere (in 4 doubles, 32 bytes)
     * bringing us to  12 + (12 + 32) = 56 bytes = 448 bits */

    /* Contains information of the rotation of the ball within "trans" */
    btTransform trans;
    engine->ballRigidBody.at(0)->getMotionState()->getWorldTransform(trans);

    /* Might need to get the position and rotation of the ball using
    the get() functions */

    // The "ints" may need to be "floats" instead, or cast
    // 12 bytes
    int bPosX = trans.getOrigin().getX();
    int bPosY = trans.getOrigin().getY();
    int bPosZ = trans.getOrigin().getZ();
    // 32 bytes
    double bRotX = trans.getRotation().getX();
    double bRotY = trans.getRotation().getY();
    double bRotZ = trans.getRotation().getZ();
    double bRotW = trans.getRotation().getW();

    // The "ints" may need to be "floats" instead, or cast
    // 12 bytes
//    int bPosX = balls[0].getPos().x;
//    int bPosY = balls[0].getPos().y;
//    int bPosZ = balls[0].getPos().z;
    // 32 bytes
//    double bRotX = balls[0].getRotation().x;
//    double bRotY = balls[0].getRotation().y;
//    double bRotZ = balls[0].getRotation().z;
//    double bRotW = balls[0].getRotation().w;

    bool update = false;
    if(lastUpdate == -1) {
        lastUpdate = getCurrentTime();
        update = true;
    } else{
        update = getCurrentTime() - lastUpdate > 16;
    }

    if (update) {
        if(isServer)
        {
            if(!connectionOpened)
            {
                IPaddress ip;
                SDLNet_ResolveHost(&ip, NULL, 1234);
                server = SDLNet_TCP_Open(&ip);
            }

            char sendBuffer[500];
            Ogre::Vector3* paddleCoords = &paddle1->position;

            // Arrays preparing to use memcpy
            float sendCoords[2] = {paddleCoords->y, paddleCoords->z};
            int sendBPos[3] = {bPosX, bPosY, bPosZ};
            double sendBRot[4] = {bRotX, bRotY, bRotZ, bRotW};
            int sendScore[2] = {p1lives, p2lives};
            while(1)
            {
                if(!connectionOpened)
                {
                    client=SDLNet_TCP_Accept(server);
                    if(client)
                    {
                        connectionOpened = true;
                        mGUI->setHostVisible(false);
                    }
                }
                if(client)
                {
                    // Memcpy functions (Arrays may need '&' symbol before them)
                    memcpy(sendBuffer, &sendCoords, sizeof(float)*2); // <-- this one
                    memcpy(&sendBuffer[sizeof(float)*2], &sendBPos, sizeof(int)*3);
                    memcpy(&sendBuffer[sizeof(float)*2 + sizeof(int)*3], &sendBRot, sizeof(double)*4);
                    memcpy(&sendBuffer[sizeof(float)*2 + sizeof(int)*3 + sizeof(double)*4], &sendScore, sizeof(int)*2);
                    SDLNet_TCP_Send(client, sendBuffer, sizeof(float)*2 + sizeof(int)*3 + sizeof(double)*4 + sizeof(int)*2);
                    break;
                }
                mKeyboard->capture();
                if(mKeyboard->isKeyDown(OIS::KC_ESCAPE))
                {
                    mShutDown = true;
                    break;
                }
            }

            char recvBuffer[100];
            SDLNet_TCP_Recv(client,recvBuffer,100);
            float recvdCoords[2];
            // Need to also receive paddle collision info to render in physics engine
            memcpy(&recvdCoords, recvBuffer, sizeof(float)*2);
            paddleCoords = &paddle2->position;
            paddleCoords->y = recvdCoords[0];
            paddleCoords->z = recvdCoords[1];
            paddle2->setPos(paddleCoords->x, paddleCoords->y, paddleCoords->z);

            //            SDLNet_TCP_Close(client);
            //            SDLNet_TCP_Close(server);
        }

        if (!isServer)
        {

            if(!connectionOpened)
            {
                IPaddress ip;
                std::cout << "Attempting to connect to " << mGUI->currentAddress << "\n";
                SDLNet_ResolveHost(&ip, mGUI->currentAddress.c_str(), 1234);
                server=SDLNet_TCP_Open(&ip);
                connectionOpened = true;
            }
            char sendBuffer[100];
            Ogre::Vector3* paddleCoords = &paddle2->position;
            float sendCoords[2] = {paddleCoords->y, paddleCoords->z};
            memcpy(sendBuffer, &sendCoords, sizeof(float)*2);
            SDLNet_TCP_Send(server,sendBuffer,sizeof(float)*2);

            char recvBuffer[500];
            SDLNet_TCP_Recv(server,recvBuffer,500);

            float recvdCoords[2];
            int recvdBPos[3];
            double recvdBRot[4];
            int recvdScore[2];

            memcpy(recvdCoords, &recvBuffer, sizeof(float)*2);
            memcpy(&recvdBPos, &recvBuffer[sizeof(float)*2], sizeof(int)*3);
            memcpy(&recvdBRot, &recvBuffer[sizeof(float)*2 + sizeof(int)*3], sizeof(double)*4);
            memcpy(&recvdScore, &recvBuffer[sizeof(float)*2 + sizeof(int)*3 + sizeof(double)*4], sizeof(int)*2);
            p1lives = recvdScore[0];
            p2lives = recvdScore[1];


            paddleCoords = &paddle1->position;
            paddleCoords->y = recvdCoords[0];
            paddleCoords->z = recvdCoords[1];
            trans.setOrigin(btVector3(recvdBPos[0],recvdBPos[1], recvdBPos[2]));
//            trans.setOrigin().setX(recvdBPos[0]);
//            trans.setOrigin().setY(recvdBPos[1]);
//            trans.setOrigin().setZ(recvdBPos[2]);
            trans.setRotation(btQuaternion(recvdBRot[0], recvdBRot[1], recvdBRot[2], recvdBRot[3]));
//            trans.setRotation().setX(recvdBRot[0]);
//            trans.setRotation().setY(recvdBRot[1]);
//            trans.setRotation().setZ(recvdBRot[2]);
//            trans.setRotation().setW(recvdBRot[3]);

//            balls[0]->setPos(recvdBPos[0], recvdBPos[1], recvdBPos[2]);
//            balls[0]->setRot(recvdBRot[0], recvdBRot[1], recvdBRot[2], recvdBRot[3]);

            balls.at(0)->setPos(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
            balls.at(0)->setRot(trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ(),
                                trans.getRotation().getW());

            paddle1->setPos(paddleCoords->x, paddleCoords->y, paddleCoords->z);

            //            SDLNet_TCP_Close(server);
        }
    }
}

//---------------------------------------------------------------------------
