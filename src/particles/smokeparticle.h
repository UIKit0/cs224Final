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

    dSpaceID space;
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
    float expandtime;
    float lifetime;

    // Parameters that control how the smoke dissipates. Calculated as exp(lifetime*base)
    float fade;     // controls alpha
    float alpha;    // opacity of the sprites
    float minAlpha; // what fade to make the particle disappear

    float shrink;   // controls size
    float expand;   // controls expanding to initial size
    float initialScale; // scale when the particle is first spawned
    float scale;     // how much to scale the size
    float minScale;  // what scale to make the particle disappear

    bool active;
    glm::vec3 getPosition();
};

#endif // SMOKEPARTICLE_H
