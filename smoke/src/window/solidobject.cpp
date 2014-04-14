#include "solidobject.h"

SolidObject::SolidObject()
{
}

SolidObject::SolidObject(dWorldID w, dSpaceID s, dMass m)
{
    body = dBodyCreate(w);
    dBodySetKinematic(body);
    dMassSetBox(&m, 10.0f, SIDE_LENGTH, HEIGHT, SIDE_LENGTH);
    geom = dCreateBox(s, SIDE_LENGTH, HEIGHT, SIDE_LENGTH);
    dGeomSetBody(geom, body);
    dGeomSetCategoryBits(geom, 4);
    dGeomSetCollideBits(geom, 1);
    dBodySetPosition(body, 10, 10, 0);
    moving = 0;

    VortexShedder* vs = new VortexShedder(w, s, m, body);
    vs->falloff = 1.0f;
    vs->force = 0.1f;
    vs->forcedecay = 2.0f;
    vs->range = HEIGHT;
    vs->rangedecay = 4.0f;
    // Don't scale too much with velocity
    vs->velScale = 0.1f;
    vs->centripetal = 3.5f;
    vs->velThreshold = 0.01f;
    shedders.append(vs);

    vs = new VortexShedder(w, s, m, body);
    vs->falloff = 1.0f;
    vs->force = 0.02f;
    vs->forcedecay = 2.0f;
    vs->range = HEIGHT;
    vs->rangedecay = 4.0f;
    // Don't scale too much with velocity
    vs->velScale = 0.01f;
    vs->centripetal = 3.5f;
    vs->velThreshold = 0.01f;
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
        dBodySetLinearVel(body, 2.0f, 0, 0);
    }
    else if (moving < 0){
        dBodySetLinearVel(body, -2.0f, 0, 0);
    }
    else{
        dBodySetLinearVel(body, 0, 0, 0);
    }
    if (dBodyGetPosition(body)[0] > 10){
        moving = -1;
        left = glm::vec3(0, 0, 1);
    }
    if (dBodyGetPosition(body)[0] < -10){
        moving = 1;
        left = glm::vec3(0, 0, -1);
    }

    const dReal *vel = dBodyGetLinearVel(body);

    VortexShedder* vs = shedders[0];
    vs->axis = glm::normalize(glm::vec3(0, 0, vel[0]));
    const dReal* location = dBodyGetPosition(body);
    glm::vec3 loc(location[0], location[1] - HEIGHT, location[2]);

    vs->location = loc;
    vs->location[0] = vs->location[0] - vel[0];
    vs->velThreshold = 0.01f;

    vs = shedders[1];
    vs->axis = glm::normalize(glm::vec3(0, 0, -vel[0]));
    location = dBodyGetPosition(body);
    loc = glm::vec3(location[0], location[1] + HEIGHT, location[2]);

    vs->location = loc;
    vs->location[0] = vs->location[0] - vel[0];
    vs->velThreshold = 0.01f;

    for (int i = 0; i < shedders.size(); i++){
        shedders[i]->update(seconds);
    }
}

void SolidObject::draw(Obj &obj) {
    glColor3f(0,1,0);
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);
    glScalef(SIDE_LENGTH, HEIGHT, SIDE_LENGTH);
    obj.draw();
    glScalef(1.0f/SIDE_LENGTH, 1.0f/HEIGHT, 1.0f/SIDE_LENGTH);
    glTranslatef(-pos[0], -pos[1], -pos[2]);

    for (int i = 0; i < shedders.size(); i++){
        shedders[i]->draw(obj);
    }
}


void SolidObject::destroy(){
    dBodyDestroy(body);
    dGeomDestroy(geom);
    for (int i = 0; i < shedders.size(); i++){
        shedders[i]->destroy();
    }
}
