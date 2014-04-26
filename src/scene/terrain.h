#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/vec3.hpp"
#include "glm/gtc/noise.hpp"
#include <QHash>
#include <iostream>

#include "scene/global.h"

#define GRID_SIZE 11
#define UPDATE_RADIUS 2
#define TILE_SIZE 10
#define NOISE_COORDINATE_RATIO 0.1f

struct Update{
    int ix;
    int iy;

    Update(int x, int y){
        ix = x;
        iy = y;
    }
};

struct Tile{
    float terrain[TILE_SIZE][TILE_SIZE];
    glm::vec3 normals[TILE_SIZE][TILE_SIZE];
    int m_buffer;
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
    virtual ~Terrain();

    void update(float seconds, glm::vec3 playerLocation);
    void updateVBO();

    void draw();

private:
    void shiftTiles(int x, int y);

    Tile *tiles[GRID_SIZE][GRID_SIZE];

    // Location of the origin, used to maintain perlin noise consistency
    glm::vec3 originLocation;

    QList<Update> updateQueue;

    GLFunctions *gl;
};

#endif // TERRAIN_H
