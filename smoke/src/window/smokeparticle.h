#ifndef SMOKEPARTICLE_H
#define SMOKEPARTICLE_H

#include <GL/glu.h>
#include <ode/ode.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#define PARTICLE_SIZE 0.5f
#define PARTICLE_CATEGORY_BITS 1
#define DAMPING_FACTOR 0.05f

class SmokeParticle
{
public:
    SmokeParticle(dWorldID world, dSpaceID space, dMass mass);

    void destroy();
    void update();
    void draw(GLUquadric* quad);

    dBodyID body;
    dGeomID geom;
};

#endif // SMOKEPARTICLE_H
