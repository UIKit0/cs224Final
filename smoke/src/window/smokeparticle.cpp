#include "smokeparticle.h"

SmokeParticle::SmokeParticle(dWorldID world, dSpaceID space, dMass mass)
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
}

void SmokeParticle::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void SmokeParticle::draw(Obj &obj){
    const dReal* pos = dBodyGetPosition(body);

    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glScalef(0.2f, 0.2f, 0.2f);
    obj.draw();
//    glTranslatef(-pos[0], -pos[1], -pos[2]);
    glPopMatrix();
}

void SmokeParticle::update(){
    dBodyAddForce(body, 0, dRandReal()*0.02f, 0);
//    if (dRandReal() < 0.1){
//        dBodyAddForce(body, dRandReal()*0.2f - 0.1f, 0, dRandReal()*0.2f - 0.1f);
//    }
    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*DAMPING_FACTOR, -vel[1]*DAMPING_FACTOR, -vel[2]*DAMPING_FACTOR);
}
