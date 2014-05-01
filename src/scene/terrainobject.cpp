#include "terrainobject.h"

TerrainObject::TerrainObject(GLFunctions *gl, glm::vec3 loc) : location(loc)
{
    m_gl = gl;
    health = 10.0f;
    radius = 0.5f;
}

void TerrainObject::onBulletHit(Bullet &bullet){
    health -= bullet.damage;
    if (health < 5.0f && particles != NULL){
        particles = new BasicSmokeEmitter(g_world, g_mass);
//        particles->initialize(m_gl);
        std::cout<<"smoking now"<<std::endl;
    }
}

void TerrainObject::update(float seconds){
    if (particles != NULL)
        particles->update(seconds);
}

void TerrainObject::draw(){
    if (particles != NULL){
        particles->draw();
    }
}
