#include "explosionemitter.h"

ExplosionEmitter::ExplosionEmitter(Particles *p) : ParticleEmitter(p)
  , duration(10.0f)
{
}

void ExplosionEmitter::updateParticles(){
    if (time > duration){
        active = false;
        return;
    }

    while (particles.size() < 200){
        SmokeParticle sp = SmokeParticle(g_world, space, g_mass, dRandReal()*0.5f + 1.5f);

        dBodySetPosition(sp.body, location[0],
                         location[1],
                         location[2]);

        glm::vec3 vel = glm::sphericalRand(dRandReal()*0.5f + 0.2f);
        dBodySetLinearVel(sp.body, vel[0], vel[1], vel[2]);

        sp.wind = glm::vec3(0,0.1f,0);
        sp.rotationSpeed = dRandReal() - 0.5f;

        sp.shrink = -2.0f - dRandReal()*0.2f;
        sp.initialScale = 0.7f;
        sp.expand = 3.0f;
        sp.minScale = 0.1f;
        sp.fade = -0.0f;
        sp.minAlpha = 0.02f;
        sp.lifetime = 100.0f;
        particles.push_front(sp);
    }
}

void ExplosionEmitter::updateVortices(){

}
