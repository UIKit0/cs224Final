#include "missile.h"

Missile::Missile(GLFunctions *gl, dSpaceID s, glm::vec3 loc, glm::vec3 vel) :
    active(true)
  , time(0)
  , damage(1.0f)
  , emitter(NULL)
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

    emitter = new SmokeTrailEmitter(g_particles);
//    emitter->initialize(gl);
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
