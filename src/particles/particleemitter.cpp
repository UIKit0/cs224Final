#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(dWorldID w, dMass m)
    : Particles(w)
{
    mass = m;
    drawVortices = false;
    sprites = UINT_MAX;
    time = 0;
}

void ParticleEmitter::draw(){
    Q_ASSERT_X(sprites != UINT_MAX, "particle emitter", "no sprites loaded");

    // Particles
//    float *m = glm::value_ptr(g_camera.vMatrix);
//    glm::vec3 dx(m[0], m[4], m[8]); // left-right
//    glm::vec3 dy(m[1], m[5], m[9]); // up-down
//    glm::vec3 dz(m[2], m[6], m[10]); // front-back

//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, sprites);

    //glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    Particles::draw();

    glDepthMask(GL_TRUE);

//    glDisable(GL_TEXTURE_2D);

//      // Vortices
//    if (drawVortices){
//        for (int i = 0; i < vortices.size(); i++){
//              vortices[i]->draw(obj);
//        }
//    }
}

void ParticleEmitter::update(float seconds){
    time += seconds*0.1;
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

    Particles::setBufferSize(particles.size());

    //update buffer
    for (int i = 0; i < particles.size(); ++i){
        Particles::setBufferValue(i, particles[i].getPosition(), particles[i].scale * 5);
    }
}

