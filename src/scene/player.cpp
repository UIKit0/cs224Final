#include "player.h"

Player::Player()
{
}

void Player::initialize(GLFunctions *gl){
    m_gl = gl;
    shader.initialize(gl, "../../../../res/shaders/");
//    shader.initialize(gl, "../res/shaders/");
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
    g_model.mMatrix = glm::translate(g_model.mMatrix, location - glm::cross(facing, left));
    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(roll), facing);
    g_model.mMatrix = glm::rotate(g_model.mMatrix, -glm::radians(rotation[0]), up);
    g_model.mMatrix = glm::rotate(g_model.mMatrix, glm::radians(rotation[1]), glm::vec3(0,0,1.0f));

    m_gl->glBindVertexArray(object_vao);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, object_vbo);

    m_gl->glUseProgram(shader.program());

    m_gl->glUniformMatrix4fv(shader.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
    m_gl->glUniformMatrix4fv(shader.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix*g_model.mMatrix));

    m_gl->glDrawArrays(GL_TRIANGLES, 0, 3*3);
    g_model.popMatrix();
}
