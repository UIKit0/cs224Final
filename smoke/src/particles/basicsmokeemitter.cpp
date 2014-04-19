#include "basicsmokeemitter.h"

BasicSmokeEmitter::BasicSmokeEmitter(dWorldID w, dSpaceID s, dMass m) : ParticleEmitter(w, s, m)
{
    perlins.append(new PerlinNoise(0.2f, 5));
    perlin = new PerlinNoise(0.2f, 5);

    QImage img = QImage(":/textures/smoke2.png");

    if (img.isNull()) {
        qCritical("Unable to load texture!");
        return;
    }

    img = img.convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &sprites);
    glBindTexture(GL_TEXTURE_2D, sprites);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    maxInitialVel = glm::vec3(0,0,0);
    minInitialVel = glm::vec3(0,0,0);
}

void BasicSmokeEmitter::updateParticles(){
//    float timescale = 0.2f;
    for (int i = 0; i < particles.size(); i++){
        // TODO: perlin noise
//        const dReal *loc = dBodyGetPosition(particles[i].body);
//        float noise = perlin->perlin_noise(loc[1], time*timescale, loc[0]*loc[0] + loc[2]*loc[2]);
//        float noise2 = perlin->perlin_noise(loc[1] + 1, time*timescale, loc[0]*loc[0] + loc[2]*loc[2]);
//        float noise3 = perlin->perlin_noise(loc[1] + 2, time*timescale, loc[0]*loc[0] + loc[2]*loc[2]);
//        dBodyAddForce(particles[i].body, noise*5, noise2*5, noise3*5);
    }
}

void BasicSmokeEmitter::updateVortices(){
    // Do nothing
}

void BasicSmokeEmitter::spawnParticles(){
    int toAdd;
    if (particles.size() > 1500)
        toAdd = 1;
    else
        toAdd = 2;

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
        sp.fade = -0.6f;
        sp.minAlpha = 0.02f;
        particles.append(sp);
    }
}

void BasicSmokeEmitter::spawnVortices(){
    // Do nothing
}

void BasicSmokeEmitter::addVortex(){
    Vortex* v = new Vortex(world, space, mass, dRandReal()*1.5f);

    // Spawn in a circle around the center
    float theta = dRandReal()*M_PI*2;
    dBodySetPosition(v->body, SPAWN_SIZE*cos(theta), dRandReal()*1.0f, SPAWN_SIZE*sin(theta));

    v->axis = glm::normalize(glm::vec3(-sin(theta), 0, cos(theta)));
    v->falloff = 3.0f;
    v->force = dRandReal()*0.007f;
    v->centripetal = 0.2f;
    v->forcedecay = 0.4f;
    v->rangedecay = 0.1f;
    vortices.append(v);
}
