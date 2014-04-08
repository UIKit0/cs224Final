#include "smokeparticle.h"

SmokeParticle::SmokeParticle(dWorldID world, dSpaceID space, dMass mass)
{
    body = dBodyCreate(world);
    geom = dCreateSphere(space, PARTICLE_SIZE);

    dGeomSetBody(geom, body);

    dGeomSetCategoryBits(geom, PARTICLE_CATEGORY_BITS);
    dGeomSetCollideBits(geom, ~PARTICLE_CATEGORY_BITS);

    // Maintain same mass
    dMassSetSphere(&mass, 1/PARTICLE_SIZE, PARTICLE_SIZE);
    dBodySetMass(body, &mass);
}

void SmokeParticle::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void SmokeParticle::draw(GLUquadric* quad){
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    gluSphere(quad, PARTICLE_SIZE, 8, 8);
    glTranslatef(-pos[0], -pos[1], -pos[2]);
}

void SmokeParticle::update(){
    dBodyAddForce(body, 0, dRandReal()*0.05f, 0);
    if (dRandReal() < 0.1){
        dBodyAddForce(body, dRandReal()*0.2f - 0.1f, 0, dRandReal()*0.2f - 0.1f);
    }
    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*0.1f, -vel[1]*0.1f, -vel[2]*0.1f);
}
