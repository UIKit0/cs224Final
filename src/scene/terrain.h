#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/vec3.hpp"
#include "glm/gtc/noise.hpp"
#include <QHash>

#include <QOpenGLFunctions>

#define GRID_SIZE 5
#define TILE_SIZE 20
#define NOISE_COORDINATE_RATIO 0.1f

struct Update{
    int ix;
    int iy;
};

struct Tile{
    float terrain[TILE_SIZE][TILE_SIZE];
    glm::vec3 normals[TILE_SIZE][TILE_SIZE];
};

/**
 * Terrain is composed of tiles, in which each tile has an
 * array of floats which represents the height of each point on the plane.
 * Tiles are loaded and unloaded when during update() based on the player position.
 */

class Terrain
{
public:
    Terrain();

    void update(float seconds, glm::vec3 playerLocation);
    void updateVBO();

    void draw();

    Tile tiles[GRID_SIZE][GRID_SIZE];

    // Location of the origin, used to maintain perlin noise consistency
    glm::vec3 originLocation;

    QList<Update> updateQueue;

};

#endif // TERRAIN_H
