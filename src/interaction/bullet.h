#ifndef BULLET_H
#define BULLET_H

#include "ode/ode.h"
#include "glm/vec3.hpp"

#include "scene/global.h"
#include "interaction/collisions.h"

#define BULLET_CATEGORY_BITS 8
#define LIFETIME 10.0f

class Bullet
{
public:
    Bullet(dSpaceID s, glm::vec3 loc, glm::vec3 vel);

    void destroy();
    void update(float seconds);

    float damage;
    bool active;

    dBodyID body;
    dGeomID geom;

    float time;
};

#endif // BULLET_H
