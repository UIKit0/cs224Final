#include "enemy.h"

Enemy::Enemy(GLFunctions *gl, dSpaceID s, glm::vec3 loc, glm::vec3 rot) :
    m_gl(gl)
  , health(5.0f)
  , active(true)
  , particles(NULL)
{
    QString f("LP_Flyingcar_02.obj");
    Obj mesh(f);
    m_obj.initialize(gl, mesh);

    body = dBodyCreate(g_world);
    dBodySetData(body, this);
    dMassSetBox(&g_mass, 100.0f, SIDE_LENGTH, HEIGHT, SIDE_LENGTH);
    dBodySetMass(body, &g_mass);
    geom = dCreateBox(s, SIDE_LENGTH, HEIGHT, SIDE_LENGTH);
    dGeomSetBody(geom, body);
    dGeomSetCategoryBits(geom, 4);
    dGeomSetCollideBits(geom, 1 | 2 | 4 | 8);
    dBodySetPosition(body, loc[0], loc[1], loc[2]);
    dBodySetLinearVel(body, sin(rot[0])*cos(rot[1]), sin(rot[1]), cos(rot[0])*cos(rot[1]));
    rotation = rot;
}

void Enemy::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void Enemy::update(float seconds){
    const dReal* loc = dBodyGetPosition(body);
    if (particles != NULL){
        particles->location = glm::vec3(loc[0], loc[1], loc[2]);
    }
//    if (moving > 0){
//        dBodySetLinearVel(body, 4.0f, 0, 0);
//    }
//    else if (moving < 0){
//        dBodySetLinearVel(body, -4.0f, 0, 0);
//    }
//    else{
//        dBodySetLinearVel(body, 0, 0, 0);
//    }
//    if (dBodyGetPosition(body)[0] > 10){
//        moving = -1;
//        left = glm::vec3(0, 0, 1);
//    }
//    if (dBodyGetPosition(body)[0] < -10){
//        moving = 1;
//        left = glm::vec3(0, 0, -1);
//    }

//    const dReal *vel = dBodyGetLinearVel(body);

//    VortexShedder* vs = shedders[0];
//    vs->axis = glm::normalize(glm::vec3(0, 0, vel[0]));
//    const dReal* location = dBodyGetPosition(body);
//    glm::vec3 loc(location[0], location[1] - HEIGHT, location[2]);

//    vs->location = loc;
//    vs->location[0] = vs->location[0] - SIDE_LENGTH*vel[0]/fabs(vel[0])/2;
//    vs->velThreshold = 0.01f;

//    vs = shedders[1];
//    vs->axis = glm::normalize(glm::vec3(0, 0, -vel[0]));
//    location = dBodyGetPosition(body);
//    loc = glm::vec3(location[0], location[1] + HEIGHT, location[2]);

//    vs->location = loc;
//    vs->location[0] = vs->location[0] - SIDE_LENGTH*vel[0]/fabs(vel[0])/2;
//    vs->velThreshold = 0.01f;

//    for (int i = 0; i < shedders.size(); i++){
//        shedders[i]->update(seconds);
//    }

//    const dReal* pos = dBodyGetPosition(body);
//    WindVolume *wv = winds[0];
//    dBodySetPosition(wv->body, pos[0] + vel[0]/5, pos[1] + HEIGHT/4, pos[2]);

//    wv = winds[1];
//    dBodySetPosition(wv->body, pos[0] + vel[0]/5, pos[1] - HEIGHT/4, pos[2]);
}

void Enemy::draw(){
    const dReal *loc = dBodyGetPosition(body);
    glm::vec3 location(loc[0], loc[1], loc[2]);

    g_model.pushMatrix();
    // Translate to location
    g_model.mMatrix = glm::translate(g_model.mMatrix, location);

//    // A little extra rotation depending on how much the plane is turning
//    g_model.mMatrix = glm::rotate(g_model.mMatrix, -glm::radians(rotation[2]/3.0f), up);
//    // Rolling effect

    // Initial orientation
    g_model.mMatrix = glm::rotate(g_model.mMatrix, -rotation[0], glm::vec3(0,1.0f,0));
    g_model.mMatrix = glm::rotate(g_model.mMatrix, rotation[1], glm::vec3(0,0,1.0f));
    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(rotation[2]/1.5f), glm::vec3(1.0f, 0, 0));

    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(-90.0f), glm::vec3(0, 1.0,0));

    g_model.mMatrix = glm::scale(g_model.mMatrix, glm::vec3(0.02f, 0.02f, 0.02f));

    m_obj.draw();

    g_model.popMatrix();

//    glColor3f(1,0,0);
//    for (int i = 0; i < winds.size(); i++){
//        glPushMatrix();
//        const dReal* loc = dBodyGetPosition(winds[i]->body);
//        glTranslatef(loc[0], loc[1], loc[2]);
//        glScalef(0.5f, HEIGHT/2, SIDE_LENGTH);
//        obj.draw();
//        glPopMatrix();
//    }
}


//void SolidObject::destroy(){
//    dBodyDestroy(body);
//    dGeomDestroy(geom);
//    for (int i = 0; i < shedders.size(); i++){
//        shedders[i]->destroy();
//    }
//}

void Enemy::onMissileHit(Missile *m){
    health -= m->damage;
    m->active = false;
    if (health < 5.0f && particles == NULL){
        std::cout<<"what"<<std::endl;
        particles = new BasicSmokeEmitter(g_particles);
    }
    if (health < 0.0f && particles != NULL){
        active = false;
        particles->active = false;
    }
}

