#ifndef VORTEX_H
#define VORTEX_H

#include <ode/ode.h>
//#include <OpenGL/glu.h>
#include <qgl.h>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include "assets/obj.h"
#define VORTEX_CATEGORY_BITS 7

class Vortex
{
public:
    Vortex(dWorldID world, dSpaceID space, dMass mass, float r);

    void update(float seconds);
    void draw(Obj &obj);
    void destroy();

    dBodyID body;
    dGeomID geom;

    bool active;

    // Vortex parameters
    glm::vec3 axis;
    float force;
    float falloff;
    float range;
    float forcedecay;
    float rangedecay;
};

#endif // VORTEX_H
