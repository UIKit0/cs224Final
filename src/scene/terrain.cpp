#include "terrain.h"

Terrain::Terrain()
{
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            for (int x = 0; x < TILE_SIZE; x++){
                for (int y = 0; y <  TILE_SIZE; y++){
                    glm::vec2 loc = glm::vec2(x + i*TILE_SIZE, y + j*TILE_SIZE);
                    tiles[i][j].terrain[x][y] = 4.0f*glm::perlin(loc*NOISE_COORDINATE_RATIO);
                }
            }
        }
    }
    originLocation = glm::vec3();
}


void Terrain::draw(){
    glColor3f(0.0f, 0.0f, 0.1f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Draw a strip for each column, skip the last one since we're looking forward
    for (int x = 0; x < GRID_SIZE*TILE_SIZE - 1; x++){
        int x2 = x + 1;
        glBegin(GL_TRIANGLE_STRIP);
        for (int y = 0; y < GRID_SIZE * TILE_SIZE; y++){
                glVertex3f(x2, tiles[x2 / TILE_SIZE][y / TILE_SIZE].terrain[x2 % TILE_SIZE][y % TILE_SIZE], y);
                glVertex3f(x, tiles[x / TILE_SIZE][y / TILE_SIZE].terrain[x % TILE_SIZE][y % TILE_SIZE], y);
        }
        glEnd();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
