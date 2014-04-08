#include "vortex.h"

Vortex::Vortex(dWorldID world, dSpaceID space, dMass mass, float r)
{
    body = dBodyCreate(world);
    geom = dCreateSphere(space, r);
    dGeomSetBody(geom, body);

    dGeomSetCategoryBits(geom, VORTEX_CATEGORY_BITS);
    dGeomSetCollideBits(geom, ~VORTEX_CATEGORY_BITS);

    dBodySetKinematic(body);

    range = r;
    active = true;
}

void Vortex::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
    active = false;
}

void Vortex::draw(GLUquadric* quad){
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    gluSphere(quad, range, 16, 16);
    glTranslatef(-pos[0], -pos[1], -pos[2]);
}

void Vortex::update(float seconds){
    float forcefactor = exp(-forcedecay*seconds);
    float rangefactor = exp(-rangedecay*seconds);
    // TODO: right thing to do?
    // Make the force weaker, do something about the range?
    force = force*forcefactor;
    range = range/rangefactor;

    // Disable the vortex if it's too weak
    if (force < 0.01f){
        destroy();
    }
    else{
        dSpaceID space = dGeomGetSpace(geom);
        dGeomDestroy(geom);
        geom = dCreateSphere(space, range);
        dGeomSetBody(geom, body);

        dGeomSetCategoryBits(geom, VORTEX_CATEGORY_BITS);
        dGeomSetCollideBits(geom, ~VORTEX_CATEGORY_BITS);

        const dReal *pos = dBodyGetPosition(body);
        dBodySetPosition(body, pos[0], pos[1] + dRandReal()*0.1f, pos[2]);
    }
}
