#ifndef TERRAINOBJECT_H
#define TERRAINOBJECT_H

/**
 * Represents a static object on the terrain.
 * Collision detection is done manually, rather than through ODE
 * for these objects because there aren't very many of them. Also
 * we can filter by tile, which greatly improves collision detection efficiency.
 */

#include "math.h"
#include "scene/global.h"
#include "particles/basicsmokeemitter.h"
#include "interaction/missile.h"
#include "terrain.h"


class Terrain;

class TerrainObject
{
public:
    TerrainObject(GLFunctions *gl, Terrain* t, Tile* tile, glm::vec3 loc);

    void update(float seconds);
    void draw();
    void onMissileHit(Missile &missile);

    /*
     * Terrain fields
     */
    Terrain* terrain;
    Tile* tile;

    /*
     * Positioning fields
     */
    // Relative to the tile
    glm::vec3 location;
    // Rotation matrix
    glm::mat4 rotation;
    // Speed of the terrain object
    glm::vec3 velocity;

    /*
     * Gameplay fields
     */
    float health;
    float radius;

    // Used only to pass onto the particles
    GLFunctions *m_gl;
    BasicSmokeEmitter *particles;
};

#endif // TERRAINOBJECT_H
