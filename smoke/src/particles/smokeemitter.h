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

#define SPAWN_SIZE 1.5f
#define MAX_HEIGHT 15

class SmokeEmitter
{
public:
    SmokeEmitter(dWorldID w, dSpaceID s, dMass m);

    void destroy();
    void update(float seconds);
    void draw(Obj &obj);

    void addBody();
    void addVortex();

    dWorldID world;
    dSpaceID space;
    dMass mass;

    QList<PerlinNoise*> perlins;

    QList<SmokeParticle> particles;

    QList<Vortex*> vortices;

    bool drawVortices;

    float time;
    unsigned int sprites;

    glm::vec3 maxInitialVel;
    glm::vec3 minInitialVel;
    glm::vec3 location;
};

#endif // SMOKEEMITTER_H
