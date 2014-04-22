#ifndef SMOKETRAILEMITTER_H
#define SMOKETRAILEMITTER_H

#include "particleemitter.h"

class SmokeTrailEmitter : public ParticleEmitter
{
public:
    SmokeTrailEmitter(dWorldID w, dSpaceID s, dMass m);

    void updateParticles();
    void updateVortices();
};

#endif // SMOKETRAILEMITTER_H
