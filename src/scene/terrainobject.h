#ifndef TERRAINOBJECT_H
#define TERRAINOBJECT_H

/**
 * Represents a static object on the terrain.
 * Collision detection is done manually, rather than through ODE
 * for these objects because there aren't very many of them. Also
 * we can filter by tile, which greatly improves collision detection efficiency.
 */

#include "scene/global.h"
#include "particles/basicsmokeemitter.h"
#include "interaction/bullet.h"

class TerrainObject
{
public:
    TerrainObject(GLFunctions *gl, glm::vec3 loc);

    void update(float seconds);
    void onBulletHit(Bullet &bullet);

    glm::vec3 location;
    GLFunctions *m_gl;
    BasicSmokeEmitter *particles;
    float health;
};

#endif // TERRAINOBJECT_H
