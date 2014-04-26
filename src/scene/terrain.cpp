#include "terrain.h"

Terrain::Terrain()
{
    originLocation = glm::vec3(-GRID_SIZE / 2.0f * TILE_SIZE, 0, -GRID_SIZE / 2.0f * TILE_SIZE);
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            tiles[i][j] = new Tile();
            for (int x = 0; x < TILE_SIZE; x++){
                for (int y = 0; y <  TILE_SIZE; y++){
                    glm::vec2 loc = glm::vec2(x + i*TILE_SIZE + originLocation[0], y + j*TILE_SIZE + originLocation[2]);
                    tiles[i][j]->terrain[x][y] = glm::vec3(x, 4.0f*glm::perlin(loc*NOISE_COORDINATE_RATIO), y);
                }
            }
        }
    }
}

Terrain::~Terrain(){
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            delete tiles[i][j];
        }
    }
}

void Terrain::initialize(GLFunctions *gl){
    m_gl = gl;

    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            m_gl->glGenVertexArrays(1, &(tiles[i][j]->vo.vao));
            m_gl->glGenBuffers(1, &(tiles[i][j]->vo.vbo));

            m_gl->glBindVertexArray(tiles[i][j]->vo.vao);

            updateVBO(i, j);
            m_gl->glEnableVertexAttribArray(0);
            m_gl->glBindBuffer(GL_ARRAY_BUFFER, tiles[i][j]->vo.vbo);
            m_gl->glVertexAttribPointer(0, sizeof(glm::vec3), GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

            m_gl->glBindBuffer(GL_ARRAY_BUFFER,0);
            m_gl->glBindVertexArray(0);
        }
    }
}

void Terrain::updateVBO(int i, int j){
    Tile* tile = tiles[i][j];
    if (tile->vo.vao == 0){
        tile->vo = vos.front();
        vos.pop_front();
    }
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, tile->vo.vbo);
    m_gl->glBufferData(GL_ARRAY_BUFFER,
                       sizeof(glm::vec3) * TILE_SIZE * TILE_SIZE, tile->terrain,
                       GL_STATIC_DRAW);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::draw(){
    glPushMatrix();
    glTranslatef(originLocation[0], originLocation[1], originLocation[2]);
    glColor3f(0.0f, 0.0f, 0.1f);
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            m_gl->glBindVertexArray(tiles[i][j]->vo.vao);
            m_gl->glDrawArrays(GL_POINTS, 0, sizeof(glm::vec3)*TILE_SIZE*TILE_SIZE);
            m_gl->glBindVertexArray(0);
        }
    }
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    // Draw a strip for each column, skip the last one since we're looking forward
//    for (int x = 0; x < GRID_SIZE*TILE_SIZE - 1; x++){
//        int x2 = x + 1;
//        glBegin(GL_TRIANGLE_STRIP);
//        for (int y = 0; y < GRID_SIZE * TILE_SIZE; y++){
//            if (tiles[x/TILE_SIZE][y/TILE_SIZE] == NULL
//                || tiles[x2/TILE_SIZE][y/TILE_SIZE] == NULL){
//                glVertex3f(x2, 0, y);
//                glVertex3f(x, 0, y);
//            }
//            else{
//                glVertex3f(x2, tiles[x2 / TILE_SIZE][y / TILE_SIZE]->terrain[x2 % TILE_SIZE][y % TILE_SIZE][1], y);
//                glVertex3f(x, tiles[x / TILE_SIZE][y / TILE_SIZE]->terrain[x % TILE_SIZE][y % TILE_SIZE][1], y);
//            }
//        }
//        glEnd();
//    }
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopMatrix();
}

void Terrain::shiftTiles(int sx, int sy){
    std::cout<<("shifting")<<sx<<" "<<sy<<std::endl;
    Q_ASSERT(sx > -GRID_SIZE && sx < GRID_SIZE && sy > -GRID_SIZE && sy < GRID_SIZE);

    int startx = 0;
    int endx = GRID_SIZE + sx;
    int dirx = -1;
    int starty = 0;
    int endy = GRID_SIZE + sy;
    int diry = -1;

    if (sx > 0){
        startx = GRID_SIZE - 1;
        endx = sx - 1;
        dirx = 1;
    }
    if (sy > 0){
        starty = GRID_SIZE - 1;
        endy = sy - 1;
        diry = 1;
    }

    if (sx != 0){
        for (int j = 0; j < GRID_SIZE; j++){
            for (int i = startx; dirx*(i - (startx - sx)) > 0; i -= dirx){
                if (tiles[i][j] != NULL){
                    vos.append(tiles[i][j]->vo);
                    delete tiles[i][j];
                }
            }
            for (int i = startx; dirx*(i - endx) > 0; i -= dirx){
                tiles[i][j] = tiles[i - sx][j];
            }
            for (int i = endx; dirx*(i - (endx - sx)) > 0; i -= dirx){
                tiles[i][j] = NULL;
                updateQueue.append(Update(i, j));
            }
        }
    }
    if (sy != 0){
        for (int i = 0; i < GRID_SIZE; i++){
            for (int j = starty; diry*(j - (starty - sy)) > 0; j -= diry){
                if (tiles[i][j] != NULL){
                    vos.append(tiles[i][j]->vo);
                    delete tiles[i][j];
                }
            }
            for (int j = starty; diry*(j - endy) > 0; j -= diry){
                tiles[i][j] = tiles[i][j - sy];
            }
            for (int j = endy; diry*(j - (endy - sy)) > 0; j -= diry){
                tiles[i][j] = NULL;
                updateQueue.append(Update(i, j));
            }
        }
    }
    originLocation += glm::vec3(-sx*TILE_SIZE, 0, -sy*TILE_SIZE);
}

void Terrain::update(float seconds, glm::vec3 playerLocation){
    glm::vec3 relLoc = playerLocation - originLocation;
    glm::vec2 grid((int)(relLoc[0] / TILE_SIZE), (int)(relLoc[2] / TILE_SIZE));
    int shiftx = 0;
    int shifty = 0;
    if (grid[0] > GRID_SIZE / 2 + UPDATE_RADIUS - 1){
        shiftx = -UPDATE_RADIUS;
    }
    if (grid[0] < GRID_SIZE / 2 - UPDATE_RADIUS + 1){
        shiftx = UPDATE_RADIUS;
    }
    if (grid[1] > GRID_SIZE / 2 + UPDATE_RADIUS - 1){
        shifty = -UPDATE_RADIUS;
    }
    if (grid[1] < GRID_SIZE / 2 - UPDATE_RADIUS + 1){
        shifty = UPDATE_RADIUS;
    }
    if (shiftx != 0 || shifty != 0)
        shiftTiles(shiftx, shifty);

    while (!updateQueue.isEmpty()){
        Update u = updateQueue.front();
        updateQueue.pop_front();
        // Already updated
        if (tiles[u.ix][u.iy] != NULL){
            continue;
        }
        tiles[u.ix][u.iy] = new Tile();
        tiles[u.ix][u.iy]->vo.vao = 0;
        tiles[u.ix][u.iy]->vo.vbo = 0;
        for (int x = 0; x < TILE_SIZE; x++){
            for (int y = 0; y <  TILE_SIZE; y++){
                glm::vec2 loc = glm::vec2(x + u.ix*TILE_SIZE + originLocation[0], y + u.iy*TILE_SIZE + originLocation[2]);
                tiles[u.ix][u.iy]->terrain[x][y] = glm::vec3(x, 4.0f*glm::perlin(loc*NOISE_COORDINATE_RATIO), y);
            }
        }
        updateVBO(u.ix, u.iy);

        break;
    }
}
