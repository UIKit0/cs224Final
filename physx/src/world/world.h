#ifndef WORLD_H
#define WORLD_H

#include <QHash>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QList>

#include "assets/obj.h"

#include "particles/basicsmokeemitter.h"
#include "world/mycontactmodification.h"
#include "world/collisiongroups.h"

#include <iostream>

// connection for physx debugger
#define PVD_HOST "127.0.0.1"
#define MAX_NUM_ACTOR_SHAPES 125
#include <PxPhysicsAPI.h>
using namespace physx;

class World : protected QOpenGLFunctions
{
public:
    World();
    virtual ~World();

    void init();
    void draw();
    void tick(float seconds);

    PxRigidDynamic *createDynamic(const PxTransform &t, const PxGeometry &geometry, const PxVec3 &velocity = PxVec3(0));

private:
    void initPhysics(bool interactive);
    void stepPhysics(bool interactive);
    void cleanupPhysics(bool interactive);

    void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows);
    void renderGeometry(const PxGeometryHolder& h);

    Obj sphereMesh;
    Obj cubeMesh;

    PxDefaultAllocator		gAllocator;
    PxDefaultErrorCallback	gErrorCallback;

    PxFoundation*			gFoundation;
    PxPhysics*				gPhysics;

    PxDefaultCpuDispatcher*	gDispatcher;
    PxScene*				gScene;

    PxMaterial*				gMaterial;

    PxVisualDebuggerConnection* gConnection;

    PxReal stackZ;

    BasicSmokeEmitter*  emitter;
    PxRigidDynamic*     ball;
};

#endif // WORLD_H
