#ifndef MISSILE_H
#define MISSILE_H

#include "ode/ode.h"
#include "glm/vec3.hpp"

#include "scene/global.h"
#include "interaction/collisions.h"
#include "particles/smoketrailemitter.h"
#include "scene/global.h"

// Collide with enemies and missiles
#define MISSILE_CATEGORY_BITS 12
// Collide with everything
#define MISSILE_COLLIDE_BITS 15
#define LIFETIME 7.0f

class Missile
{
public:
    Missile(GLFunctions *gl, dSpaceID s, glm::vec3 loc, glm::vec3 vel);

    void destroy();
    void update(float seconds);

    float damage;
    bool active;

    dBodyID body;
    dGeomID geom;

    float time;

    SmokeTrailEmitter *emitter;
};

#endif // MISSILE_H
