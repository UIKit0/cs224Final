#ifndef SMOKETRAILEMITTER_H
#define SMOKETRAILEMITTER_H

#include "particleemitter.h"
#include "particles/particles.h"
#include "scene/global.h"

class SmokeTrailEmitter : public ParticleEmitter
{
public:
    SmokeTrailEmitter(Particles *p);

    void updateParticles();
    void updateVortices();
};

#endif // SMOKETRAILEMITTER_H
