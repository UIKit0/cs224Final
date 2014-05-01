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

void TerrainObject::onMissileHit(Missile &missile){
    health -= missile.damage;
    if (health < 5.0f && particles == NULL){
        particles = new BasicSmokeEmitter(g_world, g_mass);
        particles->initialize(m_gl);
        particles->location = terrain->trueLocation(tile, location);
        std::cout<<"smoking now"<<std::endl;
    }
}

void TerrainObject::update(float seconds){
    if (particles != NULL)
        particles->update(seconds);

    if (glm::length2(velocity) == 0){
        return;
    }

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
        float height = terrain->heightInTile(tile->i, tile->j, location);
        location = glm::vec3(location[0], height, location[2]);
        glm::vec3 tangent = terrain->tangentPlaneInTile(tile->i, tile->j, location);
        rotation = glm::rotate(glm::mat4(), (float)atan2(tangent[0], 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        rotation = glm::rotate(rotation, -(float)atan2(tangent[2], 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotation = glm::rotate(rotation, -(float)atan2(velocity[2], velocity[0]), glm::vec3(0.0f, 1.0f, 0.0f));
        // TODO: rename the rotation matrix
        rotation = glm::translate(rotation, glm::vec3(0, EPSILON, 0));
    }
}

void TerrainObject::draw(){
    if (particles != NULL){
        particles->draw();
    }
}
