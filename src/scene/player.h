#ifndef PLAYER_H
#define PLAYER_H

#include "scene/global.h"
#include "graphics/program.h"

class Player
{
public:
    Player();
    void initialize(GLFunctions *gl);

    void draw();
    GLuint object_vao;
    GLuint object_vbo;

    glm::vec3 facing;
    glm::vec3 left;
    glm::vec3 up;
    glm::vec3 location;
    glm::vec2 rotation;
    float roll;

    GLFunctions *m_gl;
    Program shader;
};

#endif // PLAYER_H
