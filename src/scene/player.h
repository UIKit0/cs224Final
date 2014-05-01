#ifndef PLAYER_H
#define PLAYER_H

#include "ode/ode.h"
#include <QList>

#include "interaction/bullet.h"
#include "scene/global.h"
#include "scene/terrain.h"
#include "graphics/program.h"

#define COOLDOWN 0.3f

class Player
{
public:
    Player(dSpaceID s, Terrain* t);
    void initialize(GLFunctions *gl);

    void update(float seconds);
    void draw();
    void fire();

    glm::vec3 facing;
    glm::vec3 left;
    glm::vec3 up;
    glm::vec3 location;
    glm::vec2 rotation;
    float roll;

private:
    dWorldID world;
    dSpaceID space;
    dMass mass;

    Terrain *terrain;

    GLuint object_vao;
    GLuint object_vbo;

    GLFunctions *m_gl;
    Program shader;

    QList<Bullet*> bullets;

    float timer;
};

#endif // PLAYER_H
