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
    dBodySetPosition(body, 10, 5, 0);
    moving = 0;

    VortexShedder* vs = new VortexShedder(w, s, m, body);
    vs->falloff = 1.0f;
    vs->force = 4.0f;
    vs->forcedecay = 2.0f;
    vs->range = HEIGHT;
    vs->rangedecay = 2.0f;
    // Don't scale too much with velocity
    vs->velScale = 0.1f;
    vs->centripetal = 5.0f;
    vs->velThreshold = 0.01f;
    vs->lifetime = 2.0f;

    shedders.append(vs);

    vs = new VortexShedder(w, s, m, body);
    vs->falloff = 1.0f;
    vs->force = 4.0f;
    vs->forcedecay = 2.0f;
    vs->range = HEIGHT;
    vs->rangedecay = 1.0f;
    // Don't scale too much with velocity
    vs->velScale = 0.1f;
    vs->centripetal = 5.0f;
    vs->velThreshold = 0.01f;
    vs->lifetime = 2.0f;
    shedders.append(vs);

    dGeomID geom = dCreateBox(s, 0.5f, HEIGHT/2, SIDE_LENGTH);

    WindVolume* wv = new WindVolume(w, geom);
    wv->force = glm::vec3(0,1.0f,0);
    winds.append(wv);

    geom = dCreateBox(s, 0.5f, HEIGHT/2, SIDE_LENGTH);
    wv = new WindVolume(w, geom);
    wv->force = glm::vec3(0,-1.0f,0);
    winds.append(wv);
}

void SolidObject::stop(){
    moving = 0;
}

void SolidObject::start(){
    moving = 1;
}

void SolidObject::update(float seconds){
    if (moving > 0){
        dBodySetLinearVel(body, 4.0f, 0, 0);
    }
    else if (moving < 0){
        dBodySetLinearVel(body, -4.0f, 0, 0);
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
    vs->location[0] = vs->location[0] - SIDE_LENGTH*vel[0]/fabs(vel[0])/2;
    vs->velThreshold = 0.01f;

    vs = shedders[1];
    vs->axis = glm::normalize(glm::vec3(0, 0, -vel[0]));
    location = dBodyGetPosition(body);
    loc = glm::vec3(location[0], location[1] + HEIGHT, location[2]);

    vs->location = loc;
    vs->location[0] = vs->location[0] - SIDE_LENGTH*vel[0]/fabs(vel[0])/2;
    vs->velThreshold = 0.01f;

    for (int i = 0; i < shedders.size(); i++){
        shedders[i]->update(seconds);
    }

    const dReal* pos = dBodyGetPosition(body);
    WindVolume *wv = winds[0];
    dBodySetPosition(wv->body, pos[0] + vel[0]/5, pos[1] + HEIGHT/4, pos[2]);

    wv = winds[1];
    dBodySetPosition(wv->body, pos[0] + vel[0]/5, pos[1] - HEIGHT/4, pos[2]);
}

void SolidObject::draw(Obj &obj) {
    glColor3f(0,1,0);
    const dReal* pos = dBodyGetPosition(body);

    glPushMatrix();
        glTranslatef(pos[0], pos[1], pos[2]);
        glScalef(SIDE_LENGTH, HEIGHT, SIDE_LENGTH);
        obj.draw();
    glPopMatrix();

//    for (int i = 0; i < shedders.size(); i++){
//        shedders[i]->draw(obj);
//    }

    glColor3f(1,0,0);
    for (int i = 0; i < winds.size(); i++){
        glPushMatrix();
        const dReal* loc = dBodyGetPosition(winds[i]->body);
        glTranslatef(loc[0], loc[1], loc[2]);
        glScalef(0.5f, HEIGHT/2, SIDE_LENGTH);
        obj.draw();
        glPopMatrix();
    }
}


void SolidObject::destroy(){
    dBodyDestroy(body);
    dGeomDestroy(geom);
    for (int i = 0; i < shedders.size(); i++){
        shedders[i]->destroy();
    }
}
