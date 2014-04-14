#include "smokeemitter.h"

SmokeEmitter::SmokeEmitter(dWorldID w, dSpaceID s, dMass m)
{
    world = w;
    space = s;
    mass = m;
    drawVortices = false;
    perlins.append(new PerlinNoise(0.2f, 5));

    QImage img = QGLWidget::convertToGLFormat(QImage(":/textures/smoke.png"));
    glGenTextures(1, &sprites);
    glBindTexture(GL_TEXTURE_2D, sprites);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                    0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

    time = 0;
}

void SmokeEmitter::draw(Obj &obj){
    // Particles
    glBindTexture(GL_TEXTURE_2D, sprites);
    glEnable(GL_TEXTURE_2D);

    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    glm::vec3 dx(m[0], m[4], m[8]); // left-right
    glm::vec3 dy(m[1], m[5], m[9]); // up-down
    glm::vec3 dz(m[2], m[6], m[10]); // front-back

    glColor3f(1,0,0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
    for (int i = 0; i < particles.size(); i++){
        particles[i].draw(dx, dy, obj);
    }
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Vortices
    if (drawVortices){
        for (int i = 0; i < vortices.size(); i++){
              vortices[i]->draw(obj);
        }
    }
}

void SmokeEmitter::update(float seconds){
    time += seconds;
    for (int i = particles.size() - 1; i >= 0; i--){
        particles[i].update(seconds);
        if (!particles[i].active){
            particles[i].destroy();
            particles.removeAt(i);
        }
    }

    for (int i = vortices.size() - 1; i >= 0; i--){
        vortices[i]->update(seconds);
        if (vortices[i]->active){
            const dReal *pos = dBodyGetPosition(vortices[i]->body);
            dBodySetPosition(vortices[i]->body, pos[0], pos[1] + dRandReal()*0.12f, pos[2]);
        }
        else{
            delete vortices[i];
            vortices.removeAt(i);
        }
    }

    int toAdd;
    if (particles.size() > 1000)
        toAdd = 1;
    else
        toAdd = 2;

    for (int i = 0; i < toAdd; i++){
        addBody();
    }

//    if (dRandReal() < 0.05f)
//        addVortex();

    for (int i = vortices.size() - 1; i >= 0; i--){
        const dReal *pos = dBodyGetPosition(vortices[i]->body);
        if (pos[1] > MAX_HEIGHT){
            delete vortices[i];
            vortices.removeAt(i);
        }
    }

    // Remove from back of the list to avoid skipping elements
    for (int i = particles.size() - 1; i >= 0; i--){
        const dReal *pos = dBodyGetPosition(particles[i].body);
//        if (pos[1] > MAX_HEIGHT){
//            particles[i].destroy();
//            particles.removeAt(i);
//        }
    }
}

void SmokeEmitter::addBody(){
    SmokeParticle sp = SmokeParticle(world, space, mass,
                                        perlins[(int)(dRandReal()*perlins.size())]);

    dBodySetPosition(sp.body, dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2,
                     dRandReal(),
                     dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2);
    float maxInitialVel = 0.3f;
    float maxVerticalVel = 2.0f;
    dBodySetLinearVel(sp.body, dRandReal()*maxInitialVel*2 - maxInitialVel,
                            dRandReal()*maxVerticalVel,
                            dRandReal()*maxInitialVel*2 - maxInitialVel);
    sp.time = time;
    particles.append(sp);
}

//void SmokeEmitter::addVortex(){
//    Vortex v = Vortex(world, space, mass, dRandReal()*2 + 0.5f);

//    dBodySetPosition(v.body, dRandReal()*SPAWN_SIZE*2 - SPAWN_SIZE, dRandReal()*2.0f, dRandReal()*SPAWN_SIZE*2 - SPAWN_SIZE);

//    v.axis = glm::normalize(glm::vec3(dRandReal() - 0.5f, dRandReal() - 0.5f, dRandReal() - 0.5f));
//    v.falloff = 2.0f;
//    v.force = dRandReal()*0.5f;
//    v.forcedecay = 0.5f;
//    v.rangedecay = 0.05f;
//    vortices.append(v);
//}

void SmokeEmitter::addVortex(){
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
