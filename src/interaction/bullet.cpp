#include "bullet.h"

Bullet::Bullet(dWorldID w, dSpaceID s, dMass m, glm::vec3 loc, glm::vec3 vel) :
    active(true)
  , time(0)
{
    body = dBodyCreate(w);
    geom = dCreateSphere(s, 0.5f);
    dMassSetSphere(&m, 100.0f, 0.5f);
    dGeomSetBody(geom, body);
    dGeomSetCategoryBits(geom, BULLET_CATEGORY_BITS);
    // Don't interact with smoke
    dGeomSetCollideBits(geom, 2 + 4);
    dBodySetLinearVel(body, vel[0], vel[1], vel[2]);
    dBodySetPosition(body, loc[0], loc[1], loc[2]);
}

void Bullet::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void Bullet::update(float seconds){
    time += seconds;

    // Decay power over time/distance
    power = power*0.98f;
    if (time > LIFETIME)
        active = false;
}
