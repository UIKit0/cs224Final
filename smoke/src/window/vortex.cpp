#include "vortex.h"

Vortex::Vortex(dWorldID world, dSpaceID space, dMass mass, float r)
{
    body = dBodyCreate(world);
    geom = dCreateSphere(space, r);
    dGeomSetBody(geom, body);

    dGeomSetCategoryBits(geom, VORTEX_CATEGORY_BITS);
    dGeomSetCollideBits(geom, VORTEX_COLLIDE_BITS);

    dBodySetKinematic(body);

    range = r;
    active = true;

//    g_vortices.insert(body, this);
}

void Vortex::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
    active = false;
//    Vortex::g_vortices.remove(body);
}

void Vortex::draw(Obj &obj){
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_DST_COLOR);
    glColor3f(0.8f, 0.8f, 0.5f);
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    obj.draw();
    glTranslatef(-pos[0], -pos[1], -pos[2]);

    glDisable(GL_BLEND);
}

void Vortex::update(float seconds){
    float forcefactor = exp(-forcedecay*seconds);
    float rangefactor = exp(-rangedecay*seconds);
    // TODO: right thing to do?
    // Make the force weaker, do something about the range?
    force = force*forcefactor;
    range = range/rangefactor;

    // Disable the vortex if it's too weak
    if (force < 0.0001f){
        destroy();
    }
    else{
        dSpaceID space = dGeomGetSpace(geom);
        dGeomDestroy(geom);
        geom = dCreateSphere(space, range);
        dGeomSetBody(geom, body);

        dGeomSetCategoryBits(geom, VORTEX_CATEGORY_BITS);
        dGeomSetCollideBits(geom, VORTEX_COLLIDE_BITS);
    }
}
