#include "missile.h"

Missile::Missile(dSpaceID s, glm::vec3 loc, glm::vec3 vel) :
    active(true)
  , time(0)
  , damage(1.0f)
{
    body = dBodyCreate(g_world);
    geom = dCreateSphere(s, 0.5f);
    dMassSetSphere(&g_mass, 100.0f, 0.5f);
    dGeomSetBody(geom, body);
    dGeomSetCategoryBits(geom, MISSILE_CATEGORY_BITS);
    // Don't interact with smoke
    dGeomSetCollideBits(geom, 2 + 4);
    dBodySetLinearVel(body, vel[0], vel[1], vel[2]);
    dBodySetPosition(body, loc[0], loc[1], loc[2]);
}

void Missile::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void Missile::update(float seconds){
    time += seconds;

    // Decay power over time/distance
//    damage = damage*0.95f;
    if (time > LIFETIME)
        active = false;

}
