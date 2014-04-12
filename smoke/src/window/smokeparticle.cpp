#include "smokeparticle.h"

SmokeParticle::SmokeParticle(dWorldID world, dSpaceID space, dMass mass, PerlinNoise *perlin)
{
    body = dBodyCreate(world);
    geom = dCreateSphere(space, PARTICLE_SIZE);

    dGeomSetBody(geom, body);

    // Collide with everything except self
    dGeomSetCategoryBits(geom, PARTICLE_CATEGORY_BITS);
    dGeomSetCollideBits(geom, ~PARTICLE_CATEGORY_BITS);

    // Maintain same mass
    dMassSetSphere(&mass, 1/PARTICLE_SIZE, PARTICLE_SIZE);
    dBodySetMass(body, &mass);

    this->perlin = perlin;
    t = 0;
}

void SmokeParticle::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void SmokeParticle::draw(Obj &obj){
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    glScalef(0.2f, 0.2f, 0.2f);
    obj.draw();
    glScalef(5.0f, 5.0f, 5.0f);
    glTranslatef(-pos[0], -pos[1], -pos[2]);
}

void SmokeParticle::update(float seconds){
    t += seconds;
    dBodyAddForce(body, 0, dRandReal()*0.005f, 0);
    const dReal *loc = dBodyGetPosition(body);
//    if (dRandReal() < 0.1){
    float noise = perlin->perlin_noise(loc[0], loc[1], t*0.01f);
    float noise2 = perlin->perlin_noise(loc[0], -loc[1], t*0.01f);
        dBodyAddForce(body, noise*0.005f, 0, noise2*0.005f);
//    }
    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*DAMPING_FACTOR, -vel[1]*DAMPING_FACTOR, -vel[2]*DAMPING_FACTOR);
}
