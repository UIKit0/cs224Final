#ifndef CLOUD_H
#define CLOUD_H

/**
 * Particles are never created, so we don't need to store the world, and space and mass.
 * Clouds are also never created, so we don't need this class either.
 *
 * @brief
 */

#include <QtGui/QOpenGLFunctions>
#include <GL/glu.h>
#include <ode/ode.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QList>

#include "particles/smokeparticle.h"
#include "assets/obj.h"

#include "world/perlinnoise.h"

#include <iostream>

class Cloud
{
public:
    Cloud(dWorldID w, dMass m, PerlinNoise* perlin);

    void destroy();
    void generate(dWorldID w, dSpaceID s, dMass m, PerlinNoise* perlin);
    void update(float seconds);
    void draw(Obj &obj);

    QList<SmokeParticle> particles;

    // Space for the cloud
    dSpaceID space;

    bool drawVortices;
};

#endif // CLOUD_H
