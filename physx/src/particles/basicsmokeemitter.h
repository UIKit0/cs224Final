#ifndef BASICSMOKEEMITTER_H
#define BASICSMOKEEMITTER_H

#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>

#include "particles/particleemitter.h"

#define SPAWN_SIZE 2.0f

using namespace physx;
class BasicSmokeEmitter : public ParticleEmitter
{
public:
    BasicSmokeEmitter(PxPhysics *phys, PxScene *scene, PxMaterial *mat);

    void updateParticles();
    void updateVortices();

    glm::vec3 maxInitialVel;
    glm::vec3 minInitialVel;
};

#endif // BASICSMOKEEMITTER_H
