#ifndef BASICSMOKEEMITTER_H
#define BASICSMOKEEMITTER_H

#include <ode/ode.h>

#include <glm/gtc/noise.hpp>

#include "particles/particleemitter.h"
#include "particles/particles.h"
#include "scene/global.h"

#define SPAWN_SIZE 2.0f

class BasicSmokeEmitter : public ParticleEmitter
{
public:
    BasicSmokeEmitter(Particles *p);

    void updateParticles();
    void updateVortices();

    glm::vec3 maxInitialVel;
    glm::vec3 minInitialVel;
};

#endif // BASICSMOKEEMITTER_H
