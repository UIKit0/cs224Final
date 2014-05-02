#include "player.h"

Player::Player(dSpaceID s, Terrain *t) :
    up(0,1.0f,0),
    roll(0),
    terrain(t)
{
    space = s;
    timer = 0;
}

void Player::initialize(GLFunctions *gl){
    m_gl = gl;
    QString f("arwing.obj");
    Obj mesh(f);
    m_obj.initialize(gl, mesh);


    // TODO: load model
    shader.initialize(gl);
    shader.compile(GL_VERTEX_SHADER, "terrain.vertex");
    shader.compile(GL_FRAGMENT_SHADER, "terrain.fragment");
    shader.link();

    m_gl->glGenVertexArrays(1, &object_vao);
    m_gl->glBindVertexArray(object_vao);
    m_gl->glGenBuffers(1, &object_vbo);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, object_vbo);

    float data[3*3];
    int base = 0;
    data[base + 0] = 2;
    data[base + 1] = 0;
    data[base + 2] = 0;

    data[base + 3] = 0;
    data[base + 4] = 0;
    data[base + 5] = -1;

    data[base + 6] = 0;
    data[base + 7] = 0;
    data[base + 8] = 1;

    m_gl->glBufferData(GL_ARRAY_BUFFER,
                       sizeof(float) * 3 * 3, data,
                       GL_DYNAMIC_DRAW);
    m_gl->glEnableVertexAttribArray(shader.attrib("position"));
    m_gl->glVertexAttribPointer(shader.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
}

void Player::draw(){
    g_model.pushMatrix();
    // Translate to location
    g_model.mMatrix = glm::translate(g_model.mMatrix, location);

    // A little extra rotation depending on how much the plane is turning
    g_model.mMatrix = glm::rotate(g_model.mMatrix, -glm::radians(roll/3.0f), up);
    // Rolling effect
    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(roll/1.5f), facing);

    // Initial orientation
    g_model.mMatrix = glm::rotate(g_model.mMatrix, -glm::radians(rotation[0]), up);
    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(rotation[1] + pitch/2.0f), glm::vec3(0,0,1.0f));
    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(-90.0f), glm::vec3(0,1.0f,0));
//    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(-90.0f), glm::vec3(1.0f,0,0));

    g_model.mMatrix = glm::scale(g_model.mMatrix, glm::vec3(0.8f, 0.8f, 0.8f));

    m_obj.draw();

    g_model.popMatrix();
    // Draw bullets
    for (int i = 0; i < missiles.size(); i++){
        // TODO: draw
        const dReal *l = dBodyGetPosition(missiles[i]->body);
        glm::vec3 loc(l[0], l[1], l[2]);

        g_model.pushMatrix();

        // Translate to location
        g_model.mMatrix = glm::translate(g_model.mMatrix, loc);

        // Vertical triangles
        g_model.mMatrix = glm::scale(g_model.mMatrix, glm::vec3(0.2f,0.2f,0.2f));

        m_gl->glBindVertexArray(object_vao);
        m_gl->glBindBuffer(GL_ARRAY_BUFFER, object_vbo);

        m_gl->glUseProgram(shader.program());

        m_gl->glUniformMatrix4fv(shader.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
        m_gl->glUniform3fv(shader.uniform("terrain_color"), 1, glm::value_ptr(glm::vec3(0.0f,1.0f,0.0f)));

        for (int j = 0; j < 3; j++){
            g_model.pushMatrix();

            // Remove this: only for visualization
            g_model.mMatrix = glm::rotate(g_model.mMatrix, (float)M_PI*2.0f/3*j + missiles[i]->time*5.0f, up);
            g_model.mMatrix = glm::rotate(g_model.mMatrix, (float)M_PI/2, glm::vec3(0,0,1.0f));

            m_gl->glUniformMatrix4fv(shader.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix*g_model.mMatrix));
            m_gl->glDrawArrays(GL_TRIANGLES, 0, 3*3);

            g_model.popMatrix();
        }

        g_model.popMatrix();
    }
}

void Player::update(float seconds){
    timer += seconds;
    if (terrain->collidePoint(location - 1.5f*glm::cross(facing, left))){
        std::cout<<"smashing!"<<std::endl;
    }
    for (int i = missiles.size() - 1; i >= 0; i--){
        missiles[i]->update(seconds);
        if (missiles[i]->active){
            // Collide with terrain
            if (terrain->collideMissile(missiles[i])){
                missiles[i]->active = false;
            }
        }
        if (!missiles[i]->active){
            missiles[i]->destroy();
            missiles.removeAt(i);
        }
    }
}

void Player::fire(){
    if (timer > COOLDOWN){
        missiles.append(new Missile(m_gl, space, location - 1.5f*glm::cross(facing, left), facing*10.0f));
        missiles.last()->damage = 1.0f;
        std::cout<<"fired "<<missiles.size()<<std::endl;
        timer = 0;
    }
}
