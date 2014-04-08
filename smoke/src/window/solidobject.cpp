#include "solidobject.h"

SolidObject::SolidObject()
{
}

SolidObject::SolidObject(dWorldID w, dSpaceID s, dMass m)
{
    body = dBodyCreate(w);
    dBodySetKinematic(body);
    dMassSetSphere(&m, 10.0f, SPHERE_SIZE);
    geom = dCreateSphere(s, SPHERE_SIZE);
    dGeomSetBody(geom, body);
    dBodySetPosition(body, 10, 10, 0);
    movingPosX = false;
}

void SolidObject::stop(){
    dBodySetLinearVel(body, 0, 0, 0);
}

void SolidObject::update(float seconds){
    if (movingPosX){
        dBodySetLinearVel(body, 1.0f, 0, 0);
    }
    else{
        dBodySetLinearVel(body, -1.0f, 0, 0);
    }
    if (dBodyGetPosition(body)[0] > 10){
        movingPosX = false;
    }
    if (dBodyGetPosition(body)[0] < -10){
        movingPosX = true;
    }
}

void SolidObject::draw(GLUquadric *quad){
    glColor3f(0,1,0);
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    gluSphere(quad, SPHERE_SIZE, 16, 16);
    glTranslatef(-pos[0], -pos[1], -pos[2]);
}


void SolidObject::destroy(){
    dBodyDestroy(body);
    dGeomDestroy(geom);
}
