#include "terrainobject.h"

TerrainObject::TerrainObject(GLFunctions *gl, Terrain *t, Tile *tile, glm::vec3 loc) :
    location(loc)
  , terrain(t)
  , tile(tile)
  , particles(NULL)
  , m_gl(gl)
  , health(10.0f)
  , radius(0.5f)
  , type(Type::BUNKER)
  , active(true)
{
}

void TerrainObject::destroy(){
    if (particles != NULL){
//        particles->active = false;
//        particles->destroy();
//        // TODO: actually remove particles from the world
//        delete particles;
    }
}

void TerrainObject::onMissileHit(Missile &missile){
    health -= missile.damage;
    if (health < 5.0f && particles == NULL){
        particles = new BasicSmokeEmitter(g_world, g_mass);
        particles->initialize(m_gl);
        particles->location = terrain->trueLocation(tile, location);
    }
    if (health < 0.0f && particles != NULL){
        active = false;
        particles->active = false;
    }
}

void TerrainObject::update(float seconds){
    if (particles != NULL){
        particles->location = terrain->trueLocation(tile, location);
        particles->update(seconds);
    }

    if (glm::length2(velocity) == 0){
        return;
    }

    if (type == Type::BOAT){
        int hit = 0;
        glm::vec3 hitdir(0,0,0);
        for (int i = -BOAT_SIZE; i <= BOAT_SIZE; i++){
            for (int j = -BOAT_SIZE; j <= BOAT_SIZE; j++){
                glm::vec2 loc = terrain->perlinLocation(tile, location + glm::vec3(i, 0, j));
                if (terrain->height(loc) > MIN_Y + EPSILON){
                    hit++;
                    hitdir += glm::vec3(i, 0, j);
                }
            }
        }
        if (hit > 0){
            if (hitdir[0] == 0 && hitdir[2] == 0){
                velocity = -velocity;
            }
            else{
                glm::vec3 normal = glm::normalize(-hitdir/(float)hit);
                velocity = velocity - 2.0f*normal*glm::dot(velocity, normal);
                location += normal*0.5f;
            }
        }
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
        // TODO: rename the rotation matrix, or not, whatever
        rotation = glm::translate(rotation, glm::vec3(0, EPSILON, 0));
    }
}
