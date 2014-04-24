#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(dWorldID w, dSpaceID s, dMass m)
{
    world = w;
    space = s;
    mass = m;
    drawVortices = false;

    time = 0;
}

void ParticleEmitter::draw(Obj &obj){
    if (sprites == -1){
        std::cout<<"ERROR: no sprites loaded for particle emitter"<<std::endl;
        return;
    }

    // Particles
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    glm::vec3 dx(m[0], m[4], m[8]); // left-right
    glm::vec3 dy(m[1], m[5], m[9]); // up-down
    glm::vec3 dz(m[2], m[6], m[10]); // front-back

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprites);

    GLfloat dif[4];
    GLfloat amb[4];
    glGetLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glGetLightfv(GL_LIGHT0, GL_AMBIENT, amb);

    GLfloat zeros[] = {0.0f, 0.0f, 0.0f, 0.0f};
    GLfloat halfs[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, zeros);
    glLightfv(GL_LIGHT0, GL_AMBIENT, halfs);

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < particles.size(); i++){
        particles[i].draw(glm::normalize(dx), glm::normalize(dy), glm::normalize(dz), obj);
    }

    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    glDisable(GL_TEXTURE_2D);

    // Vortices
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

//    for (int i = vortices.size() - 1; i >= 0; i--){
//        vortices[i]->update(seconds);
//        if (!vortices[i]->active){
//            delete vortices[i];
//            vortices.removeAt(i);
//        }
//    }
}

