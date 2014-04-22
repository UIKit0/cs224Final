#include "world/world.h"

World::World() :
    sphereMesh("sphere.obj"),
    cubeMesh("cube.obj"),
    gFoundation(NULL),
    gPhysics(NULL),
    gDispatcher(NULL),
    gScene(NULL),
    gMaterial(NULL),
    gConnection(NULL),
    stackZ(10.0f)
{

}

World::~World()
{
    //find a better place
    cleanupPhysics(true);
}

void World::init()
{
    initializeOpenGLFunctions();
    initPhysics(true);

    emitter = new BasicSmokeEmitter(gPhysics, gScene, gMaterial);

    // Setup default render states
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Setup lighting
    glEnable(GL_LIGHTING);
    PxReal ambientColor[]	= { 0.2f, 0.2f, 0.2f, 0.0f };
    PxReal diffuseColor[]	= { 0.5f, 0.5f, 0.5f, 0.0f };
    PxReal specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };
    PxReal position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);
}

void World::draw()
{
    glColor4f(0.9f, 0.9f, 0.9f, 1.0f);

    PxScene* scene;
    PxGetPhysics().getScenes(&scene,1);
    PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
    if(nbActors)
    {
        std::vector<PxRigidActor*> actors(nbActors);
        scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
        renderActors(&actors[0], (PxU32)actors.size(), true);
    }

    emitter->draw(sphereMesh);

    // Draw grid
    glColor4f(0, 0, 0, 0.25);
    glBegin(GL_LINES);
    for (int s = 10, i = -s; i <= s; i++) {
        glVertex3f(i *2.0f, 0, -s *2.0f);
        glVertex3f(i *2.0f, 0, +s *2.0f);
        glVertex3f(-s *2.0f, 0, i *2.0f);
        glVertex3f(+s *2.0f, 0, i *2.0f);
    }
    glEnd();
}

PxRigidDynamic* World::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
    PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
    PxSetGroup(*dynamic, OBJECT_GROUP);

    dynamic->setAngularDamping(0.5f);
    dynamic->setLinearVelocity(velocity);
    gScene->addActor(*dynamic);
    return dynamic;
}

PxFilterFlags SmokeWorldFilterShader(
        PxFilterObjectAttributes attributes0, PxFilterData filterData0,
        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
        PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
        // generate contacts for all that were not filtered above
//    std::cout<<filterData0.word0<<" "<<filterData1.word0<<std::endl;
//        pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eMODIFY_CONTACTS;

//        // trigger the contact callback for pairs (A,B) where
//        // the filtermask of A contains the ID of B and vice versa.
//        if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
//                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
    // TODO: order matters here or not?
        if (PxGetGroupCollisionFlag(filterData0.word0, filterData1.word0)
                || PxGetGroupCollisionFlag(filterData1.word0, filterData0.word0)){
            pairFlags = PxPairFlag::eCONTACT_DEFAULT;
        }

        return PxFilterFlag::eDEFAULT;
}

void World::initPhysics(bool interactive)
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,profileZoneManager);
    if(gPhysics->getPvdConnectionManager())
    {
        gPhysics->getVisualDebugger()->setVisualizeConstraints(true);
        gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
        gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
        gConnection = PxVisualDebuggerExt::createConnection(gPhysics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
    }

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= gDispatcher;
    sceneDesc.filterShader	= SmokeWorldFilterShader;

    gScene = gPhysics->createScene(sceneDesc);
    gScene->setContactModifyCallback(new MyContactModification());

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.1f);

    ball = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(0,10,100)), PxSphereGeometry(3), *gMaterial, 10.0f);
    PxSetGroup(*ball, OBJECT_GROUP);

    PxSetGroupCollisionFlag(PARTICLE_GROUP, PARTICLE_GROUP, false);
    PxSetGroupCollisionFlag(PARTICLE_GROUP, OBJECT_GROUP, true);

    ball->setLinearVelocity(PxVec3(0,0,-10));
    gScene->addActor(*ball);
}

void World::stepPhysics(bool interactive)
{
    PX_UNUSED(interactive)
    gScene->simulate(1.0f/30.0f);
    gScene->fetchResults(true);
}

void World::cleanupPhysics(bool interactive)
{
    PX_UNUSED(interactive)
    gScene->release();
    gDispatcher->release();
    PxProfileZoneManager* profileZoneManager = gPhysics->getProfileZoneManager();
    if(gConnection != NULL)
        gConnection->release();
    gPhysics->release();
    profileZoneManager->release();
    gFoundation->release();
}

void World::renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows)
{
    PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
    const PxU32 nbShapes = ball->getNbShapes();
    PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
    ball->getShapes(shapes, nbShapes);

    for(PxU32 j=0;j<nbShapes;j++)
    {
        const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *ball));
        PxGeometryHolder h = shapes[j]->getGeometry();

        // render object
        glPushMatrix();
        glMultMatrixf((float*)&shapePose);
        glColor4f(0.0f, 0.75f, 0.0f, 1.0f);
        renderGeometry(h);
        glPopMatrix();
    }
//    for(PxU32 i=0;i<numActors;i++)
//    {
//        const PxU32 nbShapes = actors[i]->getNbShapes();
//        PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
//        actors[i]->getShapes(shapes, nbShapes);
//        bool sleeping = actors[i]->isRigidDynamic() ? actors[i]->isRigidDynamic()->isSleeping() : false;

//        for(PxU32 j=0;j<nbShapes;j++)
//        {
//            const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
//            PxGeometryHolder h = shapes[j]->getGeometry();

            // render object
//            glPushMatrix();
//            glMultMatrixf((float*)&shapePose);
//            if(sleeping)
//                glColor4f(0.75f, 1.0f, 0.75f, 1.0f);
//            else
//                glColor4f(0.0f, 0.75f, 0.0f, 1.0f);
//            renderGeometry(h);
//            glPopMatrix();

//            // notice this are really fake sahdows
//            if(shadows)
//            {
//                const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
//                const PxReal shadowMat[]={ 1,0,0,0, -shadowDir.x/shadowDir.y,0,-shadowDir.z/shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
//                glPushMatrix();
//                glMultMatrixf(shadowMat);
//                glMultMatrixf((float*)&shapePose);
//                glDisable(GL_LIGHTING);
//                glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
//                renderGeometry(h);
//                glEnable(GL_LIGHTING);
//                glPopMatrix();
//            }
//        }
//    }
}

void World::renderGeometry(const PxGeometryHolder& h)
{
    switch(h.getType())
    {
    case PxGeometryType::eBOX:
        {
            glScalef(h.box().halfExtents.x, h.box().halfExtents.y, h.box().halfExtents.z);
            glPushMatrix();
                glScalef(2,2,2);
                cubeMesh.draw();
            glPopMatrix();
        }
        break;
    case PxGeometryType::eSPHERE:
        {
            float diam = h.sphere().radius * 2;
            glPushMatrix();
                glScalef(diam, diam, diam);
                sphereMesh.draw();
            glPopMatrix();
        }
        break;
    }
}

void World::tick(float seconds)
{
    stepPhysics(true);
    emitter->update(seconds);
}
