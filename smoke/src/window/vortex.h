#ifndef VORTEX_H
#define VORTEX_H

#include <ode/ode.h>
#include <GL/glu.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <iostream>

#define VORTEX_CATEGORY_BITS 7

class Vortex
{
public:
    Vortex(dWorldID world, dSpaceID space, dMass mass, float r);

    void update(float seconds);
    void draw(GLUquadric* quad);
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
