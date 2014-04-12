#ifndef SMOKEPARTICLE_H
#define SMOKEPARTICLE_H

//#include <OpenGL/glu.h>
#include <ode/ode.h>
#include <qgl.h>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include "assets/obj.h"

#include "perlinnoise.h"

#define PARTICLE_SIZE 0.3f
#define PARTICLE_CATEGORY_BITS 1
#define DAMPING_FACTOR 0.05f

class SmokeParticle
{
public:
    SmokeParticle(dWorldID world, dSpaceID space, dMass mass, PerlinNoise *perlin);

    void destroy();
    void update(float seconds);
    void draw(Obj &obj);

    dBodyID body;
    dGeomID geom;
    PerlinNoise *perlin;

    float t;
};

#endif // SMOKEPARTICLE_H
