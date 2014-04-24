#include "basicsmokeemitter.h"

BasicSmokeEmitter::BasicSmokeEmitter(dWorldID w, dSpaceID s, dMass m) : ParticleEmitter(w, s, m)
{
    QImage img = QImage(":/textures/smoke2.png");

    if (img.isNull()) {
        qCritical("Unable to load texture!");
        return;
    }

    img = img.convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &sprites);
    glBindTexture(GL_TEXTURE_2D, sprites);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    maxInitialVel = glm::vec3(0,0,0);
    minInitialVel = glm::vec3(0,0,0);
}

void BasicSmokeEmitter::updateParticles(){
    // Spawn more particles
    int toAdd;
    if (particles.size() > 1500)
        toAdd = 1;
    else
        toAdd = 3;

    for (int i = 0; i < toAdd; i++){
        SmokeParticle sp = SmokeParticle(world, space, mass, dRandReal()*0.05f + 0.03f);

        dBodySetPosition(sp.body, location[0] + dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2,
                         location[1] + dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2,
                         location[2] + dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2);

        dBodySetLinearVel(sp.body, dRandReal()*(maxInitialVel[0] - minInitialVel[0]) + minInitialVel[0],
                                dRandReal()*(maxInitialVel[1] - minInitialVel[1]) + minInitialVel[1],
                                dRandReal()*(maxInitialVel[2] - minInitialVel[2]) + minInitialVel[2]);

        sp.wind = glm::vec3(0,0.5f,0);
        sp.rotationSpeed = dRandReal() - 0.5f;

        // Actually make particles bigger with time, so there aren't random gaps
        sp.shrink = 0.05f;
        sp.minScale = 0.1f;
        sp.fade = -0.4f;
        sp.minAlpha = 0.02f;
        particles.append(sp);
    }

    // Apply noise to particles for time-varying effect
    float timescale = 0.1f;
    float noisescale = 0.0f;
    for (int i = 0; i < particles.size(); i++){
        const dReal *loc = dBodyGetPosition(particles[i].body);

        float noise0 = glm::perlin(glm::vec3(loc[0], loc[1], time*timescale));
        float noise2 = glm::perlin(glm::vec3(loc[2], loc[1], time*timescale));
        dBodyAddForce(particles[i].body, noise0*noisescale, 0.0f, noise2*noisescale);
    }
}

void BasicSmokeEmitter::updateVortices(){
    if (vortices.size() < 10 && dRandReal() < 0.01f){
        Vortex* v = new Vortex(world, space, mass, dRandReal()*1.0f + 0.5f);

        // Spawn in a circle around the center
        float theta = dRandReal()*M_PI*2;
        // TODO: change so not at origin
        dBodySetPosition(v->body, SPAWN_SIZE*cos(theta), dRandReal()*1.0f, SPAWN_SIZE*sin(theta));
        dBodySetLinearVel(v->body, dRandReal()*0.1f - 0.05f, dRandReal()*3 + 2, dRandReal()*0.1f - 0.05f);

        v->axis = glm::normalize(glm::vec3(-sin(theta), 0, cos(theta)));
        v->falloff = 3.0f;
        v->force = dRandReal()*1.0f;
        v->centripetal = 1.0f;
        v->forcedecay = 1.0f;
        v->rangedecay = 0.1f;
        v->lifetime = 5.0f;
        vortices.append(v);
    }
}
