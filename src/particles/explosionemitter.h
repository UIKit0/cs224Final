#ifndef EXPLOSIONEMITTER_H
#define EXPLOSIONEMITTER_H

#include <ode/ode.h>
#include <glm/gtc/random.hpp>

#include "scene/global.h"
#include "particles/particleemitter.h"
#include "particles/particles.h"

class ExplosionEmitter : public ParticleEmitter
{
public:
    ExplosionEmitter(Particles *p);

    void updateParticles();
    void updateVortices();

    float duration;
};

#endif // EXPLOSIONEMITTER_H
