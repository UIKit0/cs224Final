#include "terrainobject.h"

TerrainObject::TerrainObject(GLFunctions *gl, Terrain *t, Tile *tile, glm::vec3 loc) :
    location(loc)
  , terrain(t)
  , tile(tile)
  , particles(NULL)
  , m_gl(gl)
  , health(10.0f)
  , radius(0.5f)
{
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
    location += velocity*seconds;
    int tilei = tile->i;
    int tilej = tile->j;

    if (location[0] < 0){
        tilei--;
        location += glm::vec3((float)TILE_SIZE, 0, 0);
    }
    if (location[0] > TILE_SIZE){
        tilei++;
        location -= glm::vec3((float)TILE_SIZE, 0, 0);
    }
    if (location[2] < 0 ){
        tilej--;
        location += glm::vec3(0, 0, (float)TILE_SIZE);
    }
    if (location[2] > TILE_SIZE){
        tilej++;
        location -= glm::vec3(0, 0, (float)TILE_SIZE);
    }
    tile = terrain->getTile(tilei, tilej);
    if (tile){
        location = glm::vec3(location[0], terrain->heightInTile(tile->i, tile->j, location) + EPSILON, location[2]);
    }
}

void TerrainObject::draw(){
    if (particles != NULL){
        particles->draw();
    }
}
