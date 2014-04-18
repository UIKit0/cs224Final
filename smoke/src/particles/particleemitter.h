#ifndef SMOKEEMITTER_H
#define SMOKEEMITTER_H

#include <QtGui/QOpenGLFunctions>
#include <GL/glu.h>
#include <ode/ode.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QList>
#include <QImage>
#include <QGLContext>

#include "particles/smokeparticle.h"
#include "interaction/vortex.h"
#include "assets/obj.h"

#include "world/perlinnoise.h"

class ParticleEmitter
{
public:
    ParticleEmitter(dWorldID w, dSpaceID s, dMass m);

    void destroy();
    void update(float seconds);
    void draw(Obj &obj);

    virtual void updateParticles() = 0;
    virtual void updateVortices() = 0;
    virtual void spawnParticles() = 0;
    virtual void spawnVortices() = 0;

    dWorldID world;
    dSpaceID space;
    dMass mass;

    QList<SmokeParticle> particles;

    QList<Vortex*> vortices;

    bool drawVortices;

    float time;
    unsigned int sprites;

    glm::vec3 location;
};

#endif // SMOKEEMITTER_H
