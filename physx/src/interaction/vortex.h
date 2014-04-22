#ifndef VORTEX_H
#define VORTEX_H

#include <qgl.h>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QHash>
#include <iostream>

#include "PxPhysicsAPI.h"

#include "assets/obj.h"
#include "world/collisiongroups.h"

using namespace physx;
class Vortex
{
public:
    Vortex(float r, PxVec3 pos, PxPhysics* phys, PxScene *scene, PxMaterial* mat);

    void update(float seconds);
    void draw(Obj &obj);

    PxRigidDynamic *body;

    bool active;

    // Vortex parameters
    glm::vec3 axis;
    float force;
    float falloff;
    float range;
    float centripetal;
    float forcedecay;
    float rangedecay;
    float lifetime;

    PxVec3 vel;

    float time;
};

//extern void handleVortexCollision(Vortex* v, dBodyID pbody);

#endif // VORTEX_H
