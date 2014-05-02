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
    glm::vec3 location(loc[0], loc[1], loc[2]);

    glm::vec3 diff = glm::normalize(target - location);
    float yaw = atan2(diff[0], diff[2]);
    float pitch = asin(diff[1]);

    if (yaw < 0)
        yaw = yaw + (float)M_PI*2;

    if (fabs(yaw - rotation[0]) > (float)M_PI){
        if (yaw > (float)M_PI){
            yaw = yaw - (float)M_PI*2;
        }
        else{
            yaw = yaw + (float)M_PI*2;
        }
    }
    float finalyaw = glm::mix(rotation[0], yaw, 0.1f);
    float roll = finalyaw - rotation[0];

    if (finalyaw > (float)M_PI*2)
        finalyaw -= (float)M_PI*2                                                                                                                                                                                                                                                                                                                                                                                                                                                 ;
    if (finalyaw < 0)
        finalyaw += (float)M_PI*2;

    float finalpitch = glm::mix(rotation[1], pitch, 0.1f);
    rotation = glm::vec3(finalyaw, finalpitch, roll*20.0f);

    glm::vec3 velocity( sin(rotation[0])*cos(rotation[1]), sin(rotation[1]), cos(rotation[0])*cos(rotation[1]) );
    dBodySetLinearVel(body, velocity[0], velocity[1], velocity[2]);

    if (particles != NULL){
        particles->location = location + velocity*seconds;
    }

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

    // Initial orientation
    g_model.mMatrix = glm::rotate(g_model.mMatrix, rotation[0], glm::vec3(0,1.0f,0));
    g_model.mMatrix = glm::rotate(g_model.mMatrix, -rotation[1], glm::vec3(1.0f,0,0));
    g_model.mMatrix = glm::rotate(g_model.mMatrix, -rotation[2], glm::vec3(0, 0, 1.0f));

    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(180.0f), glm::vec3(0, 1.0,0));

    g_model.mMatrix = glm::scale(g_model.mMatrix, glm::vec3(0.02f, 0.02f, 0.02f));

    m_obj.draw();

    g_model.popMatrix();
}

bool Enemy::hitsParticle(SmokeParticle *p){
    if (particles == NULL)
        return true;
    return particles->space != p->space;
}

void Enemy::onMissileHit(Missile *m){
    health -= m->damage;
    m->active = false;
    if (health < 5.0f && particles == NULL){
        std::cout<<"what"<<std::endl;
        particles = new SmokeTrailEmitter(g_particles);
    }
    if (health < 0.0f && particles != NULL){
        const dReal* loc = dBodyGetPosition(body);
        glm::vec3 location(loc[0], loc[1], loc[2]);
        ExplosionEmitter *e = new ExplosionEmitter(g_particles);
        e->location = location;
        e->duration = 0.5f;
        active = false;
        particles->active = false;
    }
}

