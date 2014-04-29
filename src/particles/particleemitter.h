#ifndef SMOKEEMITTER_H
#define SMOKEEMITTER_H

#include "scene/global.h"

#include <ode/ode.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QList>
#include <QImage>

#include "particles/smokeparticle.h"
#include "interaction/vortex.h"
#include "assets/obj.h"

#include "scene/global.h"
#include "particles.h"

class ParticleEmitter : public Particles
{
public:
    ParticleEmitter(dWorldID w, dMass m);

    void destroy();
    void update(float seconds);
    virtual void draw();

    bool drawVortices;

protected:
    virtual void updateParticles() = 0;
    virtual void updateVortices() = 0;

    dMass mass;

    QList<SmokeParticle> particles;

    QList<Vortex*> vortices;


    float time;
    GLuint sprites;

    glm::vec3 location;
};

#endif // SMOKEEMITTER_H
