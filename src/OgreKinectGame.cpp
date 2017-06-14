/*
-----------------------------------------------------------------------------
Filename:    OgreKinectGame.cpp
-----------------------------------------------------------------------------
This source file is part of the
________                           ____  __.__                      __
\_____  \    ___________   ____   |    |/ _|__| ____   ____   _____/  |_
 /   |   \  / ___\_  __ \_/ __ \  |      < |  |/    \_/ __ \_/ ___\   __\
/    |    \/ /_/  >  | \/\  ___/  |    |  \|  |   |  \  ___/\  \___|  |
\_______  /\___  /|__|    \___  > |____|__ \__|___|  /\___  >\___  >__|
        \//_____/             \/          \/       \/     \/     \/
  ________
 /  _____/_____    _____   ____
/   \  ___\__  \  /     \_/ __ \
\    \_\  \/ __ \|  Y Y  \  ___/
 \______  (____  /__|_|  /\___  >
        \/     \/      \/     \/
      Ogre Kinect Game
      https://github.com/liuloppan/KinectOgreGame/
-----------------------------------------------------------------------------
*/
#include "Stdafx.h"
#include "OgreKinectGame.h"
bool isUIvisible = false;

//-------------------------------------------------------------------------------------
OgreKinectGame::OgreKinectGame()
    : kinectController(0),
      character(0),
      accumulator(0),
      dt(0.01),
      dynamicsWorld(0),
      ogreDisplay(0),
      numBalls(0),
	  mTimeSinceLastBall(0)
{
    mInfo["About"] = "Ogre Kinect Game @2017.\n"
                     "Created for 3D Game Programming at Computer Scicence Yuan Ze University\n"
                     "Developer :\n"
                     "Lovisa Hassler\n"
                     "Rosdyana Kusuma\n"
                     "Project available on : https://github.com/liuloppan/KinectOgreGame/";
}
//-------------------------------------------------------------------------------------
OgreKinectGame::~OgreKinectGame()
{
}
//-------------------------------------------------------------------------------------
void OgreKinectGame::destroyScene()
{
    if (kinectController) {
        kinectController->unInitController();
        delete kinectController;
    }

    if (character) {
        delete character;
    }

    if (dynamicsWorld) {
        delete dynamicsWorld;
    }

    if (ogreDisplay) {
        delete ogreDisplay;
    }

    Ogre::MeshManager::getSingleton().remove("floor");

    mSceneMgr->clearScene(); // removes all nodes, billboards, lights etc.
    mSceneMgr->destroyAllCameras();
}
//-------------------------------------------------------------------------------------
bool OgreKinectGame::keyReleased(const OIS::KeyEvent &evt)
{
    return BaseApplication::keyReleased(evt);
}
//-------------------------------------------------------------------------------------
bool OgreKinectGame::mouseMoved(const OIS::MouseEvent &evt)
{
    return BaseApplication::mouseMoved(evt);
}
//-------------------------------------------------------------------------------------
bool OgreKinectGame::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if (!BaseApplication::mousePressed(evt, id)) {
        return false;
    }

    Ogre::Real offsetX = evt.state.X.abs / float(evt.state.width);
    Ogre::Real offsetY = evt.state.Y.abs / float(evt.state.height);

    if (id == OIS::MB_Left) {
        //createBall();
    } else if (id == OIS::MB_Right) {
    }

    return true;
}
//-------------------------------------------------------------------------------------

void OgreKinectGame::setupKinect(void)
{
    kinectController = new KinectController();
    kinectController->initController();
}

//-------------------------------------------------------------------------------------
void OgreKinectGame::createScene()
{
    this->setupKinect();

    // setup character
    character = new SinbadCharacterController();
    character->setupCharacter(this->mSceneMgr, this->kinectController);
    //character->getEntityNode()->rotate(Ogre::Vector3(0,1,0), Ogre::Degree(180));

    // setup shadow properties
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
    mSceneMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));

    // setup some basic lighting for our scene
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8));

    // add a bright light above the scene
    mLight = mSceneMgr->createLight();
    mLight->setType(Ogre::Light::LT_POINT);
    mLight->setPosition(-100, 800, 400);
    mLight->setSpecularColour(Ogre::ColourValue::White);

    // Bullet Physics

    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -50, 0));


    //create the character physical skeleton
    ogreDisplay = new OgreDisplay(dynamicsWorld);
    ragdoll = new SkeletonToRagdoll(mSceneMgr);
    ragdoll->createRagdoll(dynamicsWorld, character->getEntityNode());
    ragdoll->setDebugBones(false);

    // Floor
    Ogre::MeshManager::getSingleton().createPlane("floor", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::Plane(Ogre::Vector3::UNIT_Y, 0), 1000, 1000, 10, 10, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);


    Ogre::Entity *floor = mSceneMgr->createEntity("Floor", "floor");
    floor->setMaterialName("Examples/BeachStones");
    floor->setCastShadows(false);
    mSceneMgr->getRootSceneNode()->attachObject(floor);
    mSceneMgr->setSkyDome(true, "Examples/CloudySky", 10, 8);


    //btCollisionShape *groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
    btCollisionShape *groundShape = new btBoxShape(btVector3(500, 1, 500));
    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState *myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, myMotionState, groundShape, btVector3(0, 0, 0));
    groundRigidBodyCI.m_additionalDamping = true;
    btRigidBody *groundRigidBody = new btRigidBody(groundRigidBodyCI);
    groundRigidBody->setFriction(10.0f);
	groundRigidBody->setRestitution(1.0f);
    dynamicsWorld->addRigidBody(groundRigidBody);
    ragdoll->addIgnoreEventObject(groundRigidBody);

    // Color Data
    //texRenderTarget = Ogre::TextureManager::getSingleton().createManual("texRenderTarget", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    //                  Ogre::TEX_TYPE_2D, 320, 240, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

    //Ogre::Rectangle2D *mMiniScreen = new Ogre::Rectangle2D(true);
    //mMiniScreen->setCorners(0.5f, -0.5f, 1.0f, -1.0f);
    //mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));

    //Ogre::SceneNode *miniScreenNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("MiniScreenNode");
    //miniScreenNode->attachObject(mMiniScreen);
}

//-------------------------------------------------------------------------------------
bool OgreKinectGame::frameRenderingQueued(const Ogre::FrameEvent &fe)
{
    if (!BaseApplication::frameRenderingQueued(fe)) {
        return false;
    }

    kinectController->updatePerFrame(fe.timeSinceLastFrame);
    character->updatePerFrame(fe.timeSinceLastFrame);
	
	mTimeSinceLastBall += fe.timeSinceLastFrame;
	while (mTimeSinceLastBall >= 8.1){
		createBall(mTimeSinceLastBall);
		mTimeSinceLastBall -= 3;
	}
	

    // Update Color Data
    this->kinectController->showColorData(this->texRenderTarget);
    if (dynamicsWorld) {
        accumulator += fe.timeSinceLastFrame;
        if (accumulator >= dt) {
            dynamicsWorld->stepSimulation(dt);
            accumulator -= dt;
            //get all colliding objects and check for specific collisions
            dynamicsWorld->performDiscreteCollisionDetection();
            ogreDisplay->update();

            ragdoll->update();
            //std::string name = ragdoll->update();
            //if(name != "") name = name;
        }
    }

    return true;
}
//-------------------------------------------------------------------------------------
void OgreKinectGame::createBall(Ogre::Real time)
{ 
    char name[256];
    sprintf(name, "%d", numBalls++);
	float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    Ogre::Entity *ball = mSceneMgr->createEntity(name, "sphere.mesh");
    Ogre::SceneNode *ballNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	ball->setMaterialName("Bullet/Ball");
    ballNode->setPosition(x*time, 100, z*time);
    ballNode->attachObject(ball);
    ballNode->setScale(0.05, 0.05, 0.05);

    float radius = 5;
    btSphereShape *collisionShape = new btSphereShape(radius);
    btTransform startTransform;
    float friction = 10;
    btScalar mass = 25;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(ballNode->getPosition().x, ballNode->getPosition().y, ballNode->getPosition().z));
    btDefaultMotionState *triMotionState = new btDefaultMotionState(startTransform);
    btVector3 localInertia;
    collisionShape->calculateLocalInertia(mass, localInertia);
    btRigidBody::btRigidBodyConstructionInfo rbInfo = btRigidBody::btRigidBodyConstructionInfo(mass, triMotionState, collisionShape, localInertia);
    btRigidBody *triBody = new btRigidBody(rbInfo);
	triBody->setRestitution(1.0);
    ogreDisplay->createDynamicObject(ball, triBody);
}
//-------------------------------------------------------------------------------------


#include "Stdafx.h"
#include "OgreKinectGame.h"


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
    OgreKinectGame app;

    try {
        app.go();
    } catch (Ogre::Exception &e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " <<
                  e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
}

#ifdef __cplusplus
}
#endif