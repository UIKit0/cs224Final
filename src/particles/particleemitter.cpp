#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(Particles *p)
    : gl_particles(p)
    , active(true)
{
    // Create a space for this group of particles
    space = dHashSpaceCreate(0);
    dHashSpaceSetLevels(space, 1, 5);

    drawVortices = false;
    time = 0;
    g_emitters.append(this);
}

void ParticleEmitter::destroy(){
    dSpaceDestroy(space);
}

bool ParticleEmitter::isActive(){
    return active || particles.size() > 0;
}

void ParticleEmitter::draw(){
    // Particles
//    float *m = glm::value_ptr(g_camera.vMatrix);
//    glm::vec3 dx(m[0], m[4], m[8]); // left-right
//    glm::vec3 dy(m[1], m[5], m[9]); // up-down
//    glm::vec3 dz(m[2], m[6], m[10]); // front-back

//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, sprites);

    //glDepthMask(GL_FALSE);
    gl_particles->setBufferSize(particles.size());

    //update buffer
    for (int i = 0; i < particles.size(); ++i){
        gl_particles->setBufferValue(i, particles[i]->getPosition(), particles[i]->size*particles[i]->scale);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    gl_particles->draw();

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

    time += seconds;
    updateParticles();
    updateVortices();

    for (int i = particles.size() - 1; i >= 0; i--){
        particles[i]->update(seconds);
        if (!particles[i]->active){
            particles[i]->destroy();
            delete particles[i];
            particles.removeAt(i);
        }
    }

//    for (int i = vortices.size() - 1; i >= 0; i--){
//        vortices[i]->update(seconds);
//        if (!vortices[i]->active){
//            delete vortices[i];
//            vortices.removeAt(i);
//        }
//    }
}

