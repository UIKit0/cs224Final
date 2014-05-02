#include "basicsmokeemitter.h"

BasicSmokeEmitter::BasicSmokeEmitter(Particles *p) : ParticleEmitter(p)
{
    maxInitialVel = glm::vec3(0,0,0);
    minInitialVel = glm::vec3(0,0,0);
}

void BasicSmokeEmitter::updateParticles(){
    // Spawn more particles
    int toAdd;
    if (particles.size() > 100)
        toAdd = 1;
    else
        toAdd = 2;

    if (!active || dRandReal() < 0.5f)
        toAdd = 0;

    for (int i = 0; i < toAdd; i++){
        SmokeParticle sp = SmokeParticle(g_world, space, g_mass, dRandReal()*0.5f + 1.5f);

        dBodySetPosition(sp.body, location[0],
                         location[1],
                         location[2]);

        dBodySetLinearVel(sp.body, dRandReal()*(maxInitialVel[0] - minInitialVel[0]) + minInitialVel[0],
                                dRandReal()*(maxInitialVel[1] - minInitialVel[1]) + minInitialVel[1],
                                dRandReal()*(maxInitialVel[2] - minInitialVel[2]) + minInitialVel[2]);

        sp.wind = glm::vec3(0,0.2f,0);
        sp.rotationSpeed = dRandReal() - 0.5f;

        // Actually make particles bigger with time, so there aren't random gaps
        sp.shrink = -0.5f;
        sp.initialScale = 0.4f;
        sp.expand = 2.0f;
        sp.minScale = 0.1f;
        sp.fade = -0.0f;
        sp.minAlpha = 0.02f;
        sp.lifetime = 100.0f;
        particles.push_front(sp);
    }

    // Apply noise to particles for time-varying effect
    float timescale = 0.01f;
    float noisescale = 0.0f;
    for (int i = 0; i < particles.size(); i++){
        const dReal *loc = dBodyGetPosition(particles[i].body);

        float noise0 = glm::perlin(glm::vec3(loc[0], loc[1], time*timescale));
        float noise2 = glm::perlin(glm::vec3(loc[2], loc[1], time*timescale));
        dBodyAddForce(particles[i].body, noise0*noisescale, 0.0f, noise2*noisescale);
    }
}

void BasicSmokeEmitter::updateVortices(){
//    if (vortices.size() < 10 && dRandReal() < 0.01f){
//        Vortex* v = new Vortex(g_world, space, g_mass, dRandReal()*1.0f + 0.5f);

//        // Spawn in a circle around the center
//        float theta = dRandReal()*M_PI*2;
//        // TODO: change so not at origin
//        dBodySetPosition(v->body, SPAWN_SIZE*cos(theta), dRandReal()*1.0f, SPAWN_SIZE*sin(theta));
//        dBodySetLinearVel(v->body, dRandReal()*0.1f - 0.05f, dRandReal()*3 + 2, dRandReal()*0.1f - 0.05f);

//        v->axis = glm::normalize(glm::vec3(-sin(theta), 0, cos(theta)));
//        v->falloff = 3.0f;
//        v->force = dRandReal()*1.0f;
//        v->centripetal = 1.0f;
//        v->forcedecay = 1.0f;
//        v->rangedecay = 0.1f;
//        v->lifetime = 5.0f;
//        vortices.append(v);
//    }
}
