#include "terrainobject.h"

TerrainObject::TerrainObject(GLFunctions *gl, glm::vec3 loc) : location(loc)
{
    m_gl = gl;
    health = 10.0f;
}

void TerrainObject::onBulletHit(Bullet &bullet){
    health -= bullet.power;
    if (health < 5.0f && particles != NULL){
        particles = new BasicSmokeEmitter(g_world, g_mass);
        std::cout<<"smoking now"<<std::endl;
    }
}

void TerrainObject::update(float seconds){
    if (particles != NULL)
        particles->update(seconds);
}
