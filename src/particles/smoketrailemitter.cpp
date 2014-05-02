#include "smoketrailemitter.h"

SmokeTrailEmitter::SmokeTrailEmitter(Particles *p) : ParticleEmitter(p)
{
}

void SmokeTrailEmitter::updateParticles()
{
    if (!active)
        return;

    for (int i = 0; i < 1; i++){
        SmokeParticle *sp = new SmokeParticle(g_world, space, g_mass, dRandReal()*0.2f + 0.5f);

        float ratio = 0.1f;
        dBodySetPosition(sp->body, location[0] + dRandReal()*ratio - ratio/2,
                         location[1] + dRandReal()*ratio - ratio/2,
                         location[2] + dRandReal()*ratio - ratio/2);

        dBodySetLinearVel(sp->body, 0, dRandReal()*0.5f, 0);

        sp->rotationSpeed = dRandReal() - 0.5f;

        sp->shrink = -0.7f;
        sp->initialScale = 0.9f;
        sp->expand = 3.0f;
        sp->minScale = 0.1f;
        sp->fade = -0.0f;
        sp->minAlpha = 0.02f;
        sp->lifetime = 100.0f;
        particles.append(sp);
    }
}

void SmokeTrailEmitter::updateVortices()
{
    // Do nothing
}
