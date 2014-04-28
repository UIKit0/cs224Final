#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/vec3.hpp"
#include "glm/gtc/noise.hpp"
#include <QHash>
#include <iostream>

#include "scene/global.h"
#include "graphics/program.h"

#define GRID_SIZE 7
#define UPDATE_RADIUS 2
#define TILE_SIZE 20
#define NOISE_COORDINATE_RATIO 0.05f

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

struct Tile{
    // Overlap between tiles
    glm::vec3 terrain[TILE_SIZE + 1][TILE_SIZE + 1];
    glm::vec3 normals[TILE_SIZE + 1][TILE_SIZE + 1];
    VO vo;
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

    void initialize(GLFunctions *gl);
    void update(float seconds, glm::vec3 playerLocation);
    void updateVBO(int i, int j);
    void addObjects(int i, int j);

    void draw();

private:
    float noise(glm::vec2 loc);
    void shiftTiles(int x, int y);

    Tile *tiles[GRID_SIZE][GRID_SIZE];

    // Location of the origin, used to maintain perlin noise consistency
    glm::vec3 originLocation;

    QList<Update> updateQueue;

    QList<VO> vos;

    QList<glm::vec3> objects;
    GLuint object_vao;
    GLuint object_vbo;

    GLFunctions *m_gl;
    Program shader;
};

#endif // TERRAIN_H
