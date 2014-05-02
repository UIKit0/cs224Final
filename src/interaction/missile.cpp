#include "missile.h"

Missile::Missile(GLFunctions *gl, dSpaceID s, glm::vec3 loc, glm::vec3 vel) :
    active(true)
  , time(0)
  , damage(1.0f)
  , emitter(NULL)
{
    body = dBodyCreate(g_world);
    dBodySetData(body, this);
    geom = dCreateSphere(s, 1.0f);
    dMassSetSphere(&g_mass, 0.1f, 0.5f);
    dBodySetMass(body, &g_mass);
    dGeomSetBody(geom, body);
    dGeomSetCategoryBits(geom, 8 | 4);
    // Hit themselves
    dGeomSetCollideBits(geom, 4);
    dBodySetLinearVel(body, vel[0], vel[1], vel[2]);
    dBodySetPosition(body, loc[0], loc[1], loc[2]);

    emitter = new SmokeTrailEmitter(g_particles);
}

void Missile::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
    emitter->active = false;
}

void Missile::update(float seconds){
    time += seconds;

    const dReal* loc = dBodyGetPosition(body);
    emitter->location = glm::vec3(loc[0], loc[1], loc[2]);
    emitter->update(seconds);
    // Decay power over time/distance
    if (time > LIFETIME)
        active = false;
}

