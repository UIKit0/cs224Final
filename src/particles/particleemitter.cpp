#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(dWorldID w, dSpaceID s, dMass m)
{
    world = w;
    space = s;
    mass = m;
    drawVortices = false;
    sprites = UINT_MAX;
    time = 0;
}

void ParticleEmitter::draw(Obj &obj){
    Q_ASSERT_X(sprites != -1, "particle emitter", "no sprites loaded");

    // Particles
    float *m = glm::value_ptr(g_camera.vMatrix);
    glm::vec3 dx(m[0], m[4], m[8]); // left-right
    glm::vec3 dy(m[1], m[5], m[9]); // up-down
    glm::vec3 dz(m[2], m[6], m[10]); // front-back

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprites);

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < particles.size(); i++){
        particles[i].draw(glm::normalize(dx), glm::normalize(dy), glm::normalize(dz), obj);
    }

    glDepthMask(GL_TRUE);

    glDisable(GL_TEXTURE_2D);

//      // Vortices
//    if (drawVortices){
//        for (int i = 0; i < vortices.size(); i++){
//              vortices[i]->draw(obj);
//        }
//    }
}

void ParticleEmitter::update(float seconds){
    time += seconds;
    updateParticles();
    updateVortices();

    for (int i = particles.size() - 1; i >= 0; i--){
        particles[i].update(seconds);
        if (!particles[i].active){
            particles[i].destroy();
            particles.removeAt(i);
        }
    }

    for (int i = vortices.size() - 1; i >= 0; i--){
        vortices[i]->update(seconds);
        if (!vortices[i]->active){
            delete vortices[i];
            vortices.removeAt(i);
        }
    }
}

