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
    dGeomSetCategoryBits(geom, 4);
    dGeomSetCollideBits(geom, 1);
    dBodySetPosition(body, 10, 10, 0);
    moving = 0;

    // Create vortex shedder right behind the object
    float lt [] = {-1, 0, 0, 0, -1, 0, 0, 0, -1};
    VortexShedder vs = VortexShedder(w, s, m, body,
                                     glm::make_mat3(lt), glm::make_mat3(lt),
                                     0.01f, 0.01f,
                                     1.0f, 0.1f, SPHERE_SIZE);
    shedders.append(vs);
}

void SolidObject::stop(){
    moving = 0;
}

void SolidObject::start(){
    moving = 1;
}

void SolidObject::update(float seconds){
    if (moving > 0){
        dBodySetLinearVel(body, 0.1f, 0, 0);
    }
    else if (moving < 0){
        dBodySetLinearVel(body, -0.1f, 0, 0);
    }
    else{
        dBodySetLinearVel(body, 0, 0, 0);
    }
    if (dBodyGetPosition(body)[0] > 10){
        moving = -1;
    }
    if (dBodyGetPosition(body)[0] < -10){
        moving = 1;
    }

    for (int i = 0; i < shedders.size(); i++){
        shedders[i].update(seconds);
    }
}

void SolidObject::draw(Obj &obj) {
    glColor3f(0,1,0);
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    obj.draw();
    glTranslatef(-pos[0], -pos[1], -pos[2]);

    for (int i = 0; i < shedders.size(); i++){
        shedders[i].draw(obj);
    }
}


void SolidObject::destroy(){
    dBodyDestroy(body);
    dGeomDestroy(geom);
    for (int i = 0; i < shedders.size(); i++){
        shedders[i].destroy();
    }
}
