#include "terrain.h"

Terrain::Terrain()
{
    originLocation = glm::vec3(-GRID_SIZE / 2.0f * TILE_SIZE, 0, -GRID_SIZE / 2.0f * TILE_SIZE);
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            tiles[i][j] = new Tile();
            for (int x = 0; x < TILE_SIZE + 1; x++){
                for (int y = 0; y <  TILE_SIZE + 1; y++){
                    glm::vec2 loc = glm::vec2(x + i*TILE_SIZE + originLocation[0], y + j*TILE_SIZE + originLocation[2]);
                    tiles[i][j]->terrain[x][y] = glm::vec3(x, 4.0f*glm::perlin(loc*NOISE_COORDINATE_RATIO), y);
                }
            }
            addObjects(i, j);
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
//    shader.initialize(gl, "../../../../res/shaders/");
    shader.initialize(gl, "../res/shaders/");
    shader.compile(GL_VERTEX_SHADER, "terrain.vertex");
    shader.compile(GL_FRAGMENT_SHADER, "terrain.fragment");
    shader.link();

    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            m_gl->glGenVertexArrays(1, &(tiles[i][j]->vo.vao));
            m_gl->glBindVertexArray(tiles[i][j]->vo.vao);

            m_gl->glGenBuffers(1, &(tiles[i][j]->vo.vbo));
            updateVBO(i, j);
            m_gl->glBindBuffer(GL_ARRAY_BUFFER, tiles[i][j]->vo.vbo);
            m_gl->glEnableVertexAttribArray(shader.attrib("position"));
            m_gl->glVertexAttribPointer(shader.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
        }
    }

    m_gl->glGenVertexArrays(1, &object_vao);
    m_gl->glBindVertexArray(object_vao);
    m_gl->glGenBuffers(1, &object_vbo);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, object_vbo);

    float data[3*3*2];
    int base = 0;
    data[base + 0] = -1;
    data[base + 1] = 0;
    data[base + 2] = 0;

    data[base + 3] = -1;
    data[base + 4] = 1;
    data[base + 5] = 0;

    data[base + 6] = 1;
    data[base + 7] = 0.5f;
    data[base + 8] = 0;

    data[base + 9] = -1;
    data[base + 10] = 0;
    data[base + 11] = 0;

    data[base + 12] = 1;
    data[base + 13] = 0.5;
    data[base + 14] = 0;

    data[base + 15] = -1;
    data[base + 16] = 1;
    data[base + 17] = 0;
    m_gl->glBufferData(GL_ARRAY_BUFFER,
                       sizeof(float) * 3 * 3 * 2, data,
                       GL_DYNAMIC_DRAW);
    m_gl->glEnableVertexAttribArray(shader.attrib("position"));
    m_gl->glVertexAttribPointer(shader.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
}

void Terrain::updateVBO(int i, int j){
    Tile* tile = tiles[i][j];
    if (tile->vo.vao == 0){
        tile->vo = vos.front();
        vos.pop_front();
    }
    float data[TILE_SIZE*TILE_SIZE*3*3];
    for (int x = 0; x < TILE_SIZE; x++){
        for (int z = 0; z < TILE_SIZE; z++){

            int base = (x*TILE_SIZE + z)*3*3;
            data[base + 0] = x;
            data[base + 1] = tile->terrain[x][z][1];
            data[base + 2] = z;

            data[base + 3] = x;
            data[base + 4] = tile->terrain[x][z + 1][1];
            data[base + 5] = z + 1;

            data[base + 6] = x + 1;
            data[base + 7] = tile->terrain[x + 1][z + 1][1];
            data[base + 8] = z + 1;
        }
    }
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, tile->vo.vbo);
    m_gl->glBufferData(GL_ARRAY_BUFFER,
                       sizeof(float) * TILE_SIZE * TILE_SIZE * 3 * 3, data,
                       GL_DYNAMIC_DRAW);
}

void Terrain::addObjects(int i, int j){
    Tile* tile = tiles[i][j];
    for (int x = 1; x < TILE_SIZE; x++){
        for (int y = 1; y < TILE_SIZE; y++){
            float height = tile->terrain[x][y][1];
            if (height > tile->terrain[x + 1][y][1]
                    && height > tile->terrain[x + 1][y + 1][1]
                    && height > tile->terrain[x][y + 1][1]
                    && height > tile->terrain[x + 1][y - 1][1]
                    && height > tile->terrain[x][y - 1][1]
                    && height > tile->terrain[x + 1][y - 1][1]
                    && height > tile->terrain[x - 1][y][1]
                    && height > tile->terrain[x - 1][y - 1][1]){

                objects.append(glm::vec3(i*TILE_SIZE + x, height, j*TILE_SIZE + y));
            }
        }
    }
}

void Terrain::draw(){
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            if (tiles[i][j] == NULL)
                continue;
            g_model.pushMatrix();
            g_model.mMatrix = glm::translate(g_model.mMatrix, originLocation + glm::vec3(i*TILE_SIZE, 0, j*TILE_SIZE));

            m_gl->glBindVertexArray(tiles[i][j]->vo.vao);
            m_gl->glBindBuffer(GL_ARRAY_BUFFER, tiles[i][j]->vo.vbo);

            m_gl->glUseProgram(shader.program());

            m_gl->glUniformMatrix4fv(shader.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
            m_gl->glUniformMatrix4fv(shader.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix*g_model.mMatrix));

            m_gl->glDrawArrays(GL_LINES, 0, TILE_SIZE*TILE_SIZE*3*3);

            g_model.popMatrix();
        }
    }
    for (int i = 0; i < objects.size(); i++){
        g_model.pushMatrix();
        g_model.mMatrix = glm::translate(g_model.mMatrix, originLocation + objects[i]);
        m_gl->glBindVertexArray(object_vao);
        m_gl->glBindBuffer(GL_ARRAY_BUFFER, object_vbo);

        m_gl->glUseProgram(shader.program());

        m_gl->glUniformMatrix4fv(shader.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
        m_gl->glUniformMatrix4fv(shader.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix*g_model.mMatrix));

        m_gl->glDrawArrays(GL_TRIANGLES, 0, 3*3*2);
        g_model.popMatrix();
    }
}

void Terrain::shiftTiles(int sx, int sy){
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
    glm::vec3 shift(-sx*TILE_SIZE, 0, -sy*TILE_SIZE);
    for (int i = objects.size() - 1; i >= 0; i--){
        objects.replace(i, objects[i] - shift);
        // Remove objects that are out of bounds
        if (objects[i][0] < 0 || objects[i][0] > GRID_SIZE*TILE_SIZE
                || objects[i][2] < 0 || objects[i][2] > GRID_SIZE*TILE_SIZE)
            objects.removeAt(i);
    }
    originLocation += shift;
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
        if (u.pass == 2){
            // Already updated
            if (tiles[u.ix][u.iy] != NULL){
                continue;
            }
            tiles[u.ix][u.iy] = new Tile();
            tiles[u.ix][u.iy]->vo.vao = 0;
            tiles[u.ix][u.iy]->vo.vbo = 0;
            for (int x = 0; x < TILE_SIZE + 1; x++){
                for (int y = 0; y <  TILE_SIZE + 1; y++){
                    glm::vec2 loc = glm::vec2(x + u.ix*TILE_SIZE + originLocation[0], y + u.iy*TILE_SIZE + originLocation[2]);
                    tiles[u.ix][u.iy]->terrain[x][y] = glm::vec3(x, 4.0f*glm::perlin(loc*NOISE_COORDINATE_RATIO), y);
                }
            }
            updateVBO(u.ix, u.iy);

            u.pass--;
            updateQueue.push_back(u);
            break;
        }
        else if (u.pass == 1){
            addObjects(u.ix, u.iy);
            u.pass--;
            break;
        }
        else{
            // Do Nothing
        }
    }
}
