#ifndef SMOKEPARTICLE_H
#define SMOKEPARTICLE_H

//#include <OpenGL/glu.h>
#include <ode/ode.h>
#include <qgl.h>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include "assets/obj.h"

#include "perlinnoise.h"
#include <iostream>

#define PARTICLE_SIZE 0.5f
#define PARTICLE_CATEGORY_BITS 1
#define DAMPING_FACTOR 0.05f

class SmokeParticle
{
public:
    SmokeParticle(dWorldID world, dSpaceID space, dMass mass, PerlinNoise *perlin);

    void destroy();
    void update(float time);
    void draw(glm::vec3 u, glm::vec3 v, Obj &obj);

    dBodyID body;
    dGeomID geom;
    dMass mass;
    PerlinNoise *perlin;
    int type;
    float time;
    bool active;

private:
    float lifetime;
};

#endif // SMOKEPARTICLE_H
