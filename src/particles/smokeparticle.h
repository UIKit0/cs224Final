#ifndef SMOKEPARTICLE_H
#define SMOKEPARTICLE_H

#include <ode/ode.h>
#include <qgl.h>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include "assets/obj.h"

#include <iostream>

#define PARTICLE_CATEGORY_BITS 1
#define DAMPING_FACTOR 0.05f

#include "scene/global.h"

class SmokeParticle
{
public:
    SmokeParticle(dWorldID world, dSpaceID space, dMass mass, float radius);

    void destroy();
    void update(float time);
    void draw(glm::vec3 u, glm::vec3 v, glm::vec3 z, Obj &obj);

    dBodyID body;
    dGeomID geom;
    dMass mass;

    // Initial size of particle
    float size;

    // Movement parameters
    float rotation;
    float rotationSpeed;
    glm::vec3 wind;     // Force to be applied to the particle, use for buoyancy as well

    // Timers for the particle
    float time;
    float lifetime;

    // Parameters that control how the smoke dissipates. Calculated as exp(lifetime*base)
    float fade;     // controls alpha
    float alpha;    // opacity of the sprites
    float minAlpha; // what fade to make the particle disappear

    float shrink;   // controls size
    float scale;     // how much to scale the size
    float minScale;  // what scale to make the particle disappear

    bool active;
};

#endif // SMOKEPARTICLE_H
