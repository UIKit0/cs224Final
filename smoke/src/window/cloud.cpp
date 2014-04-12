#include "cloud.h"

Cloud::Cloud(dWorldID w, dMass m, PerlinNoise* perlin)
{
    space = dSimpleSpaceCreate(0);
    generate(w, space, m, perlin);
}

void Cloud::generate(dWorldID w, dSpaceID s, dMass m, PerlinNoise* perlin){
    glm::vec3 location(30.0f, 20.0f, 30.0f);

    for (int i = 0; i < 10000; i++){
        float size = 20;
        float scale = 1/5.0f;
        glm::vec3 center(size/2, size/2, size/2);
        glm::vec3 offset(dRandReal()*size, dRandReal()*size, dRandReal()*size);
        if (glm::length(offset - center) > size/2){
            i--;
            continue;
        }
        float noise = perlin->perlin_noise(offset[0]*scale, offset[1]*scale, offset[2]*scale);
        if (noise > 0){
            SmokeParticle sp = SmokeParticle(w, s, m, perlin);

            dBodySetPosition(sp.body, location[0] + offset[0],
                                        location[1] + offset[1],
                                        location[2] + offset[2]);

            particles.append(sp);
        }
    }
}

void Cloud::draw(Obj &obj){
    // Particles
    glColor3f(1,0,0);
    for (int i = 0; i < particles.size(); i++){
        particles[i].draw(obj);
    }
}

void Cloud::update(float seconds){
    //TODO: generate weak vortices occasionally and slightly move the particles.
}
