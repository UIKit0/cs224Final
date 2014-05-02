#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/vec3.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/random.hpp"
#include <QHash>
#include <iostream>
#include "scene/global.h"

#define GRID_SIZE 5
#define UPDATE_RADIUS 1
#define TILE_SIZE 30
#define NOISE_COORDINATE_RATIO 0.05f
#define MIN_Y -6.0f
#define EPSILON 0.3f
#define TRIANGLES_PER_TILE_ELEMENT 2

struct Update{
    int ix;
    int iy;
    int pass;

    Update(int x, int y){
        ix = x;
        iy = y;
        pass = 2;
    }
};

struct VO{
    GLuint vao;
    GLuint vbo;
};

class TerrainObject;

struct Tile{
    int i, j;
    // Overlap between tiles
    glm::vec3 terrain[TILE_SIZE + 1][TILE_SIZE + 1];
    glm::vec3 normals[TILE_SIZE + 1][TILE_SIZE + 1];
    VO vo;

    // Perlin noise coordinates at the corners, together they describe the tile in "perlin space"
    glm::vec2 loc0;
    glm::vec2 loc1;
    QList<TerrainObject> objects;
};


#include "graphics/program.h"
#include "terrainobject.h"
#include "interaction/missile.h"


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

    void initialize(GLFunctions *gl);
    void update(float seconds, glm::vec3 playerLocation);
    void updateVBO(int i, int j);
    void addObjects(int i, int j);

    void draw();

    bool collidePoint(glm::vec3 point);
    bool collideMissile(Missile* missile);

    float heightInTile(int i, int j, glm::vec3 location_in_tile);
    glm::vec3 tangentPlaneInTile(int i, int j, glm::vec3 location_in_tile);
    // Returns NULL if the index is out of bounds or has an empty tile
    Tile* getTile(int i, int j);
    glm::vec3 trueLocation(Tile* tile, glm::vec3 location_in_tile);
    glm::vec2 perlinLocation(Tile* tile, glm::vec3 location_in_tile);
    float height(glm::vec2 loc);

private:
    // Height for an element of a tile
    float height(Tile *tile, int i, int j);
    float raw_noise(glm::vec2 loc);
    void shiftTiles(int x, int y);

    Tile *tiles[GRID_SIZE][GRID_SIZE];

    // Location of the origin, used to maintain perlin noise consistency
    glm::vec3 originLocation;

    QList<Update> updateQueue;

    QList<VO> vos;

    GLuint object_vao;
    GLuint object_vbo;

    GLFunctions *m_gl;
    Program shader;
};

#endif // TERRAIN_H
