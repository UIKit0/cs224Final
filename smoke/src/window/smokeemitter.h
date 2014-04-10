#ifndef SMOKEEMITTER_H
#define SMOKEEMITTER_H

#include <QtGui/QOpenGLFunctions>
#include <GL/glu.h>
#include <ode/ode.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QList>

#include "smokeparticle.h"
#include "vortex.h"
#include "assets/obj.h"

#define SPAWN_SIZE 1.5f
#define MAX_HEIGHT 20

class SmokeEmitter
{
public:
    SmokeEmitter();
    SmokeEmitter(dWorldID w, dSpaceID s, dMass m);

    void destroy();
    void update(float seconds);
    void draw(Obj &obj);

    void addBody();
    void addVortex();

    dWorldID world;
    dSpaceID space;
    dMass mass;

    QList<SmokeParticle> particles;

    QList<Vortex> vortices;

    bool drawVortices;
};

#endif // SMOKEEMITTER_H
