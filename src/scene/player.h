#ifndef PLAYER_H
#define PLAYER_H

#include "ode/ode.h"
#include <QList>

#include "interaction/missile.h"
#include "scene/global.h"
#include "scene/terrain.h"
#include "graphics/program.h"
#include "scene/contour.h"
#include "particles/explosionemitter.h"

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
    float pitch;

    bool active;

private:
    dWorldID world;
    dSpaceID space;
    dMass mass;

    Terrain *terrain;

    GLuint object_vao;
    GLuint object_vbo;

    Contour m_obj;
    GLFunctions *m_gl;
    Program shader;

    QList<Missile*> missiles;

    float timer;
};

#endif // PLAYER_H
