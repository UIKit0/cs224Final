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
#include "interaction/missile.h"
#include "particles/particleemitter.h"
#include "particles/basicsmokeemitter.h"
#include "particles/explosionemitter.h"

#define ENEMY_CATEGORY_BITS 4

class Enemy
{
public:
    Enemy(GLFunctions *gl, dSpaceID s, glm::vec3 loc, glm::vec3 rot);

    void destroy();
    void update(float seconds);
    void draw();
    void onMissileHit(Missile* m);

    dBodyID body;
    dGeomID geom;

    // Yaw, pitch, roll
    glm::vec3 rotation;

    // Can be player or somewhere else
    glm::vec3 target;

    QList<VortexShedder*> shedders;
    QList<WindVolume*> winds;

    float health;
    bool active;

private:
    GLFunctions *m_gl;
    Program shader;
    Contour m_obj;

    GLuint object_vao;
    GLuint object_vbo;

    ParticleEmitter *particles;
};

#endif // ENEMY_H
