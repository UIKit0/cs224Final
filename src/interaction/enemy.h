#ifndef ENEMY_H
#define ENEMY_H

#include <ode/ode.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include "assets/obj.h"

#define SIDE_LENGTH 1.0f
#define HEIGHT 0.5f

#include "scene/global.h"
#include "scene/contour.h"
#include "graphics/program.h"
#include "interaction/vortexshedder.h"
#include "interaction/windvolume.h"

class Enemy
{
public:
    Enemy(GLFunctions *gl, dSpaceID s, glm::vec3 loc, glm::vec3 rot);

//    void update(float seconds);

    void draw();

    dBodyID body;
    dGeomID geom;

    // Yaw, pitch, roll
    glm::vec3 rotation;

    QList<VortexShedder*> shedders;
    QList<WindVolume*> winds;

private:
    GLFunctions *m_gl;
    Program shader;
    Contour m_obj;

    GLuint object_vao;
    GLuint object_vbo;
};

#endif // ENEMY_H
