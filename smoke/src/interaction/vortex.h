#ifndef VORTEX_H
#define VORTEX_H

#include <ode/ode.h>
//#include <OpenGL/glu.h>
#include <qgl.h>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QHash>

#include <iostream>
#include "assets/obj.h"

#define VORTEX_CATEGORY_BITS 2
// Only collide with particles
#define VORTEX_COLLIDE_BITS 1

class Vortex
{
public:
    Vortex(dWorldID world, dSpaceID space, dMass mass, float r);
    virtual ~Vortex();

    void update(float seconds);
    void draw(Obj &obj);

    dBodyID body;
    dGeomID geom;

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

    float time;
};

extern void handleVortexCollision(Vortex* v, dBodyID pbody);

#endif // VORTEX_H
