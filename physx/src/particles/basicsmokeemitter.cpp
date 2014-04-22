#include "basicsmokeemitter.h"

BasicSmokeEmitter::BasicSmokeEmitter(PxPhysics *phys, PxScene* scene, PxMaterial *mat) : ParticleEmitter(phys, scene, mat)
{
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
    // Spawn more particles
    int toAdd;
    if (particles.size() > 1500)
        toAdd = 1;
    else
        toAdd = 2;

    for (int i = 0; i < toAdd; i++){
        PxVec3 location(glm::linearRand(-SPAWN_SIZE, SPAWN_SIZE),
                        glm::linearRand(0.0f, 1.0f),
                        glm::linearRand(-SPAWN_SIZE, SPAWN_SIZE));

        SmokeParticle sp = SmokeParticle(0.05f, location, phys, scene, material);

        sp.body->setLinearVelocity(PxVec3(glm::linearRand(-1.0f, 1.0f),
                                          glm::linearRand(0.1f, 2.0f),
                                          glm::linearRand(-1.0f, 1.0f)));

        sp.wind = glm::vec3(0,0.5f,0);
        sp.rotationSpeed = glm::linearRand(-0.5f, 0.5f);

        // Actually make particles bigger with time, so there aren't random gaps
        sp.shrink = 0.2f;
        sp.minScale = 0.1f;
        sp.fade = -0.45f;
        sp.minAlpha = glm::linearRand(0.005f, 0.01f);
        particles.append(sp);
    }

    // Apply noise to particles for time-varying effect
    float timescale = 0.1f;
    float noisescale = 0.2f;
    for (int i = 0; i < particles.size(); i++){
        PxVec3 loc = particles[i].body->getGlobalPose().p;

        float noise0 = glm::perlin(glm::vec3(loc.x, loc.y, time*timescale));
        float noise2 = glm::perlin(glm::vec3(loc.z, loc.y, time*timescale));
        particles[i].body->addForce(PxVec3(noise0*noisescale, 0.0f, noise2*noisescale));
    }
}

void BasicSmokeEmitter::updateVortices(){
}
