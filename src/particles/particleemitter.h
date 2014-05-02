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

class ParticleEmitter // : public Particles
{
public:
    ParticleEmitter(Particles *p);

    void destroy();
    void update(float seconds);
    virtual void draw();

    bool drawVortices;

    glm::vec3 location;
    bool active;

protected:
    // To be shared with subclasses
    dSpaceID space;

    Particles *gl_particles;

    virtual void updateParticles() = 0;
    virtual void updateVortices() = 0;

    QList<SmokeParticle> particles;

    QList<Vortex*> vortices;

    float time;
};

#endif // SMOKEEMITTER_H
