#include "smoketrailemitter.h"

SmokeTrailEmitter::SmokeTrailEmitter(dWorldID w, dMass m) : ParticleEmitter(w, m)
{
}

void SmokeTrailEmitter::updateParticles()
{
    for (int i = 0; i < 1; i++){
        SmokeParticle sp = SmokeParticle(world, space, mass, dRandReal()*0.2f + 1.0f);

        dBodySetPosition(sp.body, location[0] + dRandReal() - 0.5f,
                         location[1] + dRandReal() - 0.5f,
                         location[2] + dRandReal() - 0.5f);

        sp.rotationSpeed = dRandReal() - 0.5f;

        // Actually make particles bigger with time, so there aren't random gaps
        sp.shrink = -0.1f;
        sp.minScale = 0.1f;
        sp.fade = -0.5f;
        sp.minAlpha = 0.02f;
        particles.append(sp);
    }
}

void SmokeTrailEmitter::updateVortices()
{
    // Do nothing
}
