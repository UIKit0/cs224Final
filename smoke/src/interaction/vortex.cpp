#include "vortex.h"

QHash<dBodyID, Vortex*> g_vortices;

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

    g_vortices.insert(body, this);

    time = 0;
}

Vortex::~Vortex(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
    g_vortices.remove(body);
}

void Vortex::draw(Obj &obj){
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_DST_COLOR);
    glColor3f(0.8f, 0.8f, 0.5f);
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    glScalef(range*2, range*2, range*2);
    obj.draw();
    glScalef(0.5f/range, 0.5f/range, 0.5f/range);
    glTranslatef(-pos[0], -pos[1], -pos[2]);

    glDisable(GL_BLEND);
}

void Vortex::update(float seconds){
    time += seconds;
    if (time > lifetime){
        active = false;
        return;
    }
    float forcefactor = exp(-forcedecay*seconds);
    float rangefactor = exp(-rangedecay*seconds);
    // TODO: right thing to do?
    // Make the force weaker, do something about the range?
    force = force*forcefactor;
    range = range/rangefactor;

    // Disable the vortex if it's too weak
    if (force < 0.01f){
        active = false;
    }
    else{
//        dSpaceID space = dGeomGetSpace(geom);
//        dGeomDestroy(geom);
//        geom = dCreateSphere(space, range);
//        dGeomSetBody(geom, body);

//        dGeomSetCategoryBits(geom, VORTEX_CATEGORY_BITS);
//        dGeomSetCollideBits(geom, VORTEX_COLLIDE_BITS);
        const dReal *vel = dBodyGetLinearVel(body);
        dBodySetLinearVel(body, vel[0]*0.95f, vel[1]*0.95f, vel[2]*0.95f);
    }
}
