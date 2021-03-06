#include "terrain.h"

Terrain::Terrain()
{
}

Terrain::~Terrain(){
    // FIX: this causing issues when quitting (bad access)!
//    for (int i = 0; i < GRID_SIZE; i++){
//        for (int j = 0; j < GRID_SIZE; j++){
//            if (tiles[i][j] != NULL)
//                delete tiles[i][j];
//        }
//    }
}

void Terrain::initialize(GLFunctions *gl){
    originLocation = glm::vec3(-GRID_SIZE / 2.0f * TILE_SIZE, 0, -GRID_SIZE / 2.0f * TILE_SIZE);
    m_gl = gl;

    QString f1("boat.obj");
    Obj mesh1(f1);
    object_objs[Type::BOAT].initialize(gl, mesh1);
    object_objs[Type::BOAT].transform = glm::scale(glm::mat4(), glm::vec3(0.5f,0.5f,0.5f));


    QString f2("tank.obj");
    Obj mesh2(f2);
    object_objs[Type::TANK].initialize(gl, mesh2);
    object_objs[Type::TANK].transform = glm::rotate(glm::mat4(), -0.3f, glm::vec3(0,1.0f,0));
    object_objs[Type::TANK].transform = glm::rotate(object_objs[Type::TANK].transform, (float)M_PI/2, glm::vec3(0,0,1.0f));
    object_objs[Type::TANK].transform = glm::rotate(object_objs[Type::TANK].transform, (float)M_PI/2, glm::vec3(0,1.0f,0));
    object_objs[Type::TANK].transform = glm::scale(object_objs[Type::TANK].transform, glm::vec3(0.25f,0.25f,0.25f));

    QString f3("SHED.obj");
    Obj mesh3(f3);
    object_objs[Type::BUNKER].initialize(gl, mesh3);
    object_objs[Type::BUNKER].transform = glm::scale(glm::mat4(), glm::vec3(10,10,10));


    QString f4("SimpleTower.obj");
    Obj mesh4(f4);
    object_objs[Type::BUILDING].initialize(gl, mesh4);
    object_objs[Type::BUILDING].transform = glm::rotate(glm::mat4(), -(float)M_PI/2, glm::vec3(1.0f, 0, 0  ));
    object_objs[Type::BUILDING].transform = glm::translate(object_objs[Type::BUILDING].transform, glm::vec3(0, -1.0f, 0));
    object_objs[Type::BUILDING].transform = glm::scale(object_objs[Type::BUILDING].transform, glm::vec3(0.003f,0.003f,0.003f));


    shader.initialize(gl);
    shader.compile(GL_VERTEX_SHADER, "terrain.vertex");
    shader.compile(GL_FRAGMENT_SHADER, "terrain.fragment");
    shader.link();

    // Generate the terrain
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            tiles[i][j] = new Tile();
            tiles[i][j]->vo.vao = 0;
            tiles[i][j]->vo.vbo = 0;
            tiles[i][j]->loc0 = glm::vec2(originLocation[0] + i*TILE_SIZE, originLocation[1] + j*TILE_SIZE);
            tiles[i][j]->loc1 = glm::vec2(originLocation[0] + i*TILE_SIZE + TILE_SIZE, originLocation[1] + j*TILE_SIZE + TILE_SIZE);
            tiles[i][j]->i = i;
            tiles[i][j]->j = j;
            for (int x = 0; x < TILE_SIZE + 1; x++){
                for (int y = 0; y <  TILE_SIZE + 1; y++){
                    tiles[i][j]->terrain[x][y] = glm::vec3(x, height(tiles[i][j], x, y), y);
                }
            }
            addObjects(i, j);
        }
    }

    // Update the VBOs for the terrain
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

    // Triangle used for player and everything else
    m_gl->glGenVertexArrays(1, &object_vao);
    m_gl->glBindVertexArray(object_vao);
    m_gl->glGenBuffers(1, &object_vbo);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, object_vbo);

    // Data for placeholder triangle
    float data[3*3*2];
    int base = 0;
    data[base + 0] = -1;
    data[base + 1] = 0;
    data[base + 2] = -0.5f;

    data[base + 3] = -1;
    data[base + 4] = 0;
    data[base + 5] = 0.5f;

    data[base + 6] = 1;
    data[base + 7] = 0;
    data[base + 8] = 0;

    data[base + 9] = 1;
    data[base + 10] = 0;
    data[base + 11] = 0;

    data[base + 12] = -1;
    data[base + 13] = 0;
    data[base + 14] = 0.5f;

    data[base + 15] = -1;
    data[base + 16] = 0;
    data[base + 17] = -0.5f;
    m_gl->glBufferData(GL_ARRAY_BUFFER,
                       sizeof(float) * 3 * 3 * 2, data,
                       GL_DYNAMIC_DRAW);
    m_gl->glEnableVertexAttribArray(shader.attrib("position"));
    m_gl->glVertexAttribPointer(shader.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);

}

float Terrain::height(Tile *tile, int i, int j){
    glm::vec2 loc = (tile->loc1 - tile->loc0) * glm::vec2((float)i,(float)j) / (float)TILE_SIZE + tile->loc0;
    return height(loc);
}

float Terrain::height(glm::vec2 loc){
    return fmax(MIN_Y, 10.0f*raw_noise(loc));
}

float Terrain::raw_noise(glm::vec2 loc){
    return glm::perlin(loc*NOISE_COORDINATE_RATIO)
                      + 0.1f*glm::perlin(loc*NOISE_COORDINATE_RATIO*5.0f);
                      + 0.01f*glm::perlin(loc*NOISE_COORDINATE_RATIO*20.0f);
}

void Terrain::updateVBO(int i, int j){
    Tile* tile = tiles[i][j];
    if (tile->vo.vao == 0){
        tile->vo = vos.front();
        vos.pop_front();
    }
    float data[TILE_SIZE*TILE_SIZE*3*3*TRIANGLES_PER_TILE_ELEMENT];
    for (int x = 0; x < TILE_SIZE; x++){
        for (int z = 0; z < TILE_SIZE; z++){

            int base = (x*TILE_SIZE + z)*3*3*TRIANGLES_PER_TILE_ELEMENT;
            data[base + 0] = x;
            data[base + 1] = tile->terrain[x][z][1];
            data[base + 2] = z;

            data[base + 3] = x;
            data[base + 4] = tile->terrain[x][z + 1][1];
            data[base + 5] = z + 1;

            data[base + 6] = x + 1;
            data[base + 7] = tile->terrain[x + 1][z + 1][1];
            data[base + 8] = z + 1;

            data[base + 9] = x;
            data[base + 10] = tile->terrain[x][z][1];
            data[base + 11] = z;

            data[base + 12] = x + 1;
            data[base + 13] = tile->terrain[x + 1][z + 1][1];
            data[base + 14] = z + 1;

            data[base + 15] = x + 1;
            data[base + 16] = tile->terrain[x + 1][z][1];
            data[base + 17] = z;
        }
    }
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, tile->vo.vbo);
    m_gl->glBufferData(GL_ARRAY_BUFFER,
                       sizeof(float) * TILE_SIZE * TILE_SIZE * 3 * 3 * TRIANGLES_PER_TILE_ELEMENT, data,
                       GL_DYNAMIC_DRAW);
}

void Terrain::addObjects(int i, int j){
    Tile* tile = getTile(i, j);
    Q_ASSERT(tile != NULL);
    // Bunkers on the hills
    for (int x = 1; x < TILE_SIZE; x++){
        for (int y = 1; y < TILE_SIZE; y++){
            float height = tile->terrain[x][y][1];
            // TODO: change height threshold
            if (    rand() / (float) RAND_MAX > 0.97f
                    && height > 1.5f
                    && height > tile->terrain[x + 1][y][1]
                    && height > tile->terrain[x + 1][y + 1][1]
                    && height > tile->terrain[x + 1][y - 1][1]
                    && height > tile->terrain[x][y + 1][1]
                    && height > tile->terrain[x][y - 1][1]
                    && height > tile->terrain[x - 1][y + 1][1]
                    && height > tile->terrain[x - 1][y][1]
                    && height > tile->terrain[x - 1][y - 1][1]){

                tile->objects.append(TerrainObject(m_gl, this, tile, glm::vec3(x, height + EPSILON, y)));
//                tile->objects.last().rotation = glm::rotate(glm::mat4(), (float)M_PI/2.0f, glm::vec3(1.0f,0,0));
            }
            glm::vec3 tangent = tangentPlaneInTile(i, j, glm::vec3(x, 0, y));
            if ((fabs(tangent[0]) > 0.65f || fabs(tangent[2]) > 0.65f) && dRandReal() > 0.997f){
                tile->objects.append(TerrainObject(m_gl, this, tile, glm::vec3(x, height + EPSILON, y)));
                tile->objects.last().rotation = glm::rotate(glm::mat4(), (float)M_PI/2.0f, glm::vec3(1.0f,0,0));
                tile->objects.last().type = Type::BUILDING;
            }
            if (dRandReal() > 0.9993f){
                tile->objects.append(TerrainObject(m_gl, this, tile, glm::vec3(x, height + EPSILON, y)));
                tile->objects.last().velocity = glm::vec3(dRandReal()*2.0f, 0, dRandReal()*2.0f);
                tile->objects.last().type = Type::TANK;
            }
        }
    }

    // Boats on the lake
    for (int x = BOAT_SIZE*2; x < TILE_SIZE - BOAT_SIZE*2; x += 2){
        for (int y = BOAT_SIZE*2; y < TILE_SIZE - BOAT_SIZE*2; y += 2){
            bool canPlace = true;
            for (int ii = -BOAT_SIZE*2; ii <= BOAT_SIZE*2; ii++){
                for (int jj = -BOAT_SIZE*2; jj <= BOAT_SIZE*2; jj++){
                    if (tile->terrain[x + ii][y + jj][1] != MIN_Y){
                        canPlace = false;
                        break;
                    }
                }
            }
            if (canPlace && rand() / (float)RAND_MAX > 0.5f){
                tile->objects.append(TerrainObject(m_gl, this, tile, glm::vec3(x, tile->terrain[x][y][1] + EPSILON, y)));
                tile->objects.last().velocity = glm::vec3(dRandReal()*2.0f, 0, dRandReal()*2.0f);
                tile->objects.last().type = Type::BOAT;
            }
        }
    }
}

void Terrain::draw(){
    glm::vec3 color;
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

            // Draw outline of triangles
            color = glm::vec3(0.0f,0.0f,0.0f);
            m_gl->glUniform3fv(shader.uniform("terrain_color"), 1, glm::value_ptr(color));
            m_gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m_gl->glDrawArrays(GL_TRIANGLES, 0, TILE_SIZE*TILE_SIZE*3*3*TRIANGLES_PER_TILE_ELEMENT);

            // Fill triangles as not to be see through
            color = glm::vec3(1.0f,1.0f,1.0f);
            m_gl->glUniform3fv(shader.uniform("terrain_color"), 1, glm::value_ptr(color));
            m_gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            m_gl->glDrawArrays(GL_TRIANGLES, 0, TILE_SIZE*TILE_SIZE*3*3*TRIANGLES_PER_TILE_ELEMENT);

            g_model.popMatrix();

            // Draw the objects in the tile
            color = glm::vec3(1.0f,0.0f,0.0f);
            for (int k = 0; k < tiles[i][j]->objects.size(); k++){
                g_model.pushMatrix();
                g_model.mMatrix = glm::translate(g_model.mMatrix, originLocation +
                                                        glm::vec3(i*TILE_SIZE, 0, j*TILE_SIZE) +
                                                        tiles[i][j]->objects[k].location);
                g_model.mMatrix *= tiles[i][j]->objects[k].rotation;

                m_gl->glBindVertexArray(object_vao);
                m_gl->glBindBuffer(GL_ARRAY_BUFFER, object_vbo);

                m_gl->glUseProgram(shader.program());

                m_gl->glUniformMatrix4fv(shader.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
                m_gl->glUniformMatrix4fv(shader.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix*g_model.mMatrix));

                m_gl->glUniform3fv(shader.uniform("terrain_color"), 1, glm::value_ptr(color));
//                m_gl->glDrawArrays(GL_TRIANGLES, 0, 3*3*2);

                if (tiles[i][j]->objects[k].type == Type::TANK) {
                    m_gl->glDisable(GL_CULL_FACE);
                }

                object_objs[tiles[i][j]->objects[k].type].draw();

                if (tiles[i][j]->objects[k].type == Type::TANK) {
                    m_gl->glEnable(GL_CULL_FACE);
                }

                g_model.popMatrix();
            }
        }
    }
}

void Terrain::shiftTiles(int sx, int sy){
    Q_ASSERT(sx > -GRID_SIZE && sx < GRID_SIZE && sy > -GRID_SIZE && sy < GRID_SIZE);

    // Only shift when the previous update is finished
    if (!updateQueue.isEmpty())
        return;

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
                tiles[i][j]->i = i;
                tiles[i][j]->j = j;
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
                tiles[i][j]->i = i;
                tiles[i][j]->j = j;
            }
            for (int j = endy; diry*(j - (endy - sy)) > 0; j -= diry){
                tiles[i][j] = NULL;
                updateQueue.append(Update(i, j));
            }
        }
    }
    glm::vec3 shift(-sx*TILE_SIZE, 0, -sy*TILE_SIZE);
    originLocation += shift;
}

void Terrain::update(float seconds, glm::vec3 playerLocation){
    for (int i = 0; i < GRID_SIZE; i++){
        for (int j = 0; j < GRID_SIZE; j++){
            Tile *tile = getTile(i,j);
            if (tile != NULL){
                for (int k = tile->objects.size() - 1; k >= 0; k--){
                    tile->objects[k].update(seconds);
                    if (!tile->objects[k].active){
                        tile->objects.removeAt(k);
                    }
                    else if (tile->objects[k].tile != tile){
                        if (tile->objects[k].tile != NULL)
                            tile->objects[k].tile->objects.append(tile->objects[k]);
                        tile->objects.removeAt(k);
                    }
                }
            }
        }
    }

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
            Tile* tile = tiles[u.ix][u.iy];
            tile->vo.vao = 0;
            tile->vo.vbo = 0;
            tile->i = u.ix;
            tile->j = u.iy;

            float minx, miny, maxx, maxy;
            minx = miny = maxx = maxy = FLT_MAX;
            // Check adjacent neighbors to see if locations are already determined
            if (getTile(u.ix + 1, u.iy) != NULL){
                maxx = tiles[u.ix + 1][u.iy]->loc0[0];
                maxy = tiles[u.ix + 1][u.iy]->loc1[1];
                miny = tiles[u.ix + 1][u.iy]->loc0[1];
            }
            if (getTile(u.ix - 1, u.iy) != NULL){
                minx = tiles[u.ix - 1][u.iy]->loc1[0];
                maxy = tiles[u.ix - 1][u.iy]->loc1[1];
                miny = tiles[u.ix - 1][u.iy]->loc0[1];
            }
            if (getTile(u.ix, u.iy + 1) != NULL){
                maxy = tiles[u.ix][u.iy + 1]->loc0[1];
                maxx = tiles[u.ix][u.iy + 1]->loc1[0];
                minx = tiles[u.ix][u.iy + 1]->loc0[0];
            }
            if (getTile(u.ix, u.iy - 1) != NULL){
                miny = tiles[u.ix][u.iy - 1]->loc1[1];
                maxx = tiles[u.ix][u.iy - 1]->loc1[0];
                minx = tiles[u.ix][u.iy - 1]->loc0[0];
            }
            // Check corner touching tiles, turns out to not actually be necessary
//            if (getTile(u.ix + 1, u.iy + 1) != NULL){
//                maxx = tiles[u.ix + 1][u.iy + 1]->loc0[0];
//                maxy = tiles[u.ix + 1][u.iy + 1]->loc0[1];
//            }
//            if (getTile(u.ix + 1, u.iy - 1) != NULL){
//                maxx = tiles[u.ix + 1][u.iy - 1]->loc0[0];
//                miny = tiles[u.ix + 1][u.iy - 1]->loc1[1];
//            }
//            if (getTile(u.ix - 1, u.iy + 1) != NULL){
//                minx = tiles[u.ix - 1][u.iy + 1]->loc1[0];
//                maxy = tiles[u.ix - 1][u.iy + 1]->loc0[1];
//            }
//            if (getTile(u.ix - 1, u.iy - 1) != NULL){
//                minx = tiles[u.ix - 1][u.iy - 1]->loc1[0];
//                miny = tiles[u.ix - 1][u.iy - 1]->loc1[1];
//            }

            if (minx == FLT_MAX){
                if (maxx == FLT_MAX){
                    std::cout<<"x min and max both unbounded, this is bad 0"<<std::endl;
                    // Try to update it later
                    delete tiles[u.ix][u.iy];
                    tiles[u.ix][u.iy] = NULL;
                    updateQueue.push_back(u);
                    break;
                }
                minx = maxx - TILE_SIZE*glm::linearRand(0.4f, 1.5f);
            }
            if (maxx == FLT_MAX){
                if (minx == FLT_MAX){
                    std::cout<<"x min and max both unbounded, this is bad 1"<<std::endl;
                    delete tiles[u.ix][u.iy];
                    tiles[u.ix][u.iy] = NULL;
                    updateQueue.push_back(u);
                    break;
                }
                maxx = minx + TILE_SIZE*glm::linearRand(0.4f, 1.5f);
            }
            if (miny == FLT_MAX){
                if (maxy == FLT_MAX){
                    std::cout<<"y min and max both unbounded, this is bad"<<std::endl;
                    delete tiles[u.ix][u.iy];
                    tiles[u.ix][u.iy] = NULL;
                    updateQueue.push_back(u);
                    break;
                }
                miny = maxy - TILE_SIZE*glm::linearRand(0.4f, 1.5f);
            }
            if (maxy == FLT_MAX){
                if (miny == FLT_MAX){
                    std::cout<<"y min and max both unbounded, this is bad"<<std::endl;
                    delete tiles[u.ix][u.iy];
                    tiles[u.ix][u.iy] = NULL;
                    updateQueue.push_back(u);
                    break;
                }
                maxy = miny + TILE_SIZE*glm::linearRand(0.4f, 1.5f);
            }

            tile->loc0 = glm::vec2(minx, miny);
            tile->loc1 = glm::vec2(maxx, maxy);
            for (int x = 0; x < TILE_SIZE + 1; x++){
                for (int y = 0; y <  TILE_SIZE + 1; y++){
                    tiles[u.ix][u.iy]->terrain[x][y] = glm::vec3(x, height(tiles[u.ix][u.iy], x, y), y);
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
            // Should never happen
            Q_ASSERT(false);
        }
    }
}

// TODO: make this also check for collisions with objects
bool Terrain::collidePoint(glm::vec3 point){
    Tile* tile = getTile((int)(point[0] - originLocation[0]) / TILE_SIZE,
                          (int)(point[2] - originLocation[2]) / TILE_SIZE);
    if (tile != NULL){
        return tile->terrain[(int)(point[0] - originLocation[0]) % TILE_SIZE][(int)(point[2] - originLocation[2]) % TILE_SIZE][1] >
                     point[1] - originLocation[1] - 1.0f;
    }
    return false;
}

// Ignores the y-component of location_in_tile
float Terrain::heightInTile(int i, int j, glm::vec3 location_in_tile){
    Tile* tile = getTile(i,j);

    Q_ASSERT(tile);
    glm::vec2 loc = perlinLocation(tile, location_in_tile);
    return height(loc);
}

glm::vec3 Terrain::tangentPlaneInTile(int i, int j, glm::vec3 location_in_tile){
    Tile* tile = getTile(i,j);

    Q_ASSERT(tile);
    glm::vec2 loc = perlinLocation(tile, location_in_tile);

    float eps = 0.1f;
    // Calculate x and z derivatives
    float h0 = height(loc - glm::vec2(eps, 0));
    float h1 = height(loc + glm::vec2(eps, 0));
    float h2 = height(loc - glm::vec2(0, eps));
    float h3 = height(loc + glm::vec2(0, eps));

    return glm::vec3((h1 - h0)/eps/2.0f, 0, (h3 - h2)/eps/2.0f);
}

bool Terrain::collideMissile(Missile *missile){
    const dReal *loc = dBodyGetPosition(missile->body);
    glm::vec3 point(loc[0], loc[1], loc[2]);
    glm::vec3 point_in_terrain = point - originLocation;
    int i = (int)point_in_terrain[0] / TILE_SIZE;
    int j = (int)point_in_terrain[2] / TILE_SIZE;

    Tile* tile = getTile(i,j);
    if (tile != NULL){
        glm::vec3 point_in_tile = point_in_terrain - glm::vec3(i*(float)TILE_SIZE, 0.0f, j*(float)TILE_SIZE);
        for (int i = 0; i < tile->objects.size(); i++){
            if (tile->objects[i].radius*tile->objects[i].radius > glm::length2(point_in_tile - tile->objects[i].location)){
                tile->objects[i].onMissileHit(*missile);
                return true;
            }
        }
        if (tile->terrain[(int)(point[0] - originLocation[0]) % TILE_SIZE][(int)(point[2] - originLocation[2]) % TILE_SIZE][1] > point[1] - originLocation[1]){
            return true;
        }
    }
    return false;
}

Tile* Terrain::getTile(int i, int j){
    if (i < 0 || i > GRID_SIZE - 1 || j < 0 || j > GRID_SIZE - 1)
        return NULL;

    return tiles[i][j];
}

glm::vec3 Terrain::trueLocation(Tile* tile, glm::vec3 location_in_tile){
    return originLocation + glm::vec3((float)tile->i, 0, (float)tile->j) * (float)TILE_SIZE + location_in_tile;
}

glm::vec2 Terrain::perlinLocation(Tile* tile, glm::vec3 location_in_tile){
    return tile->loc0 + (tile->loc1 - tile->loc0) * glm::vec2(location_in_tile[0], location_in_tile[2]) * (1.0f/TILE_SIZE);
}
