#ifndef WORLD_H
#define WORLD_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <QHash>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QImage>

#include "assets/obj.h"
#include "scene/camera.h"
#include "scene/global.h"
#include "graphics/program.h"

#include "interaction/vortex.h"
#include "interaction/solidobject.h"
#include "particles/smokeparticle.h"
#include "particles/basicsmokeemitter.h"
#include "particles/smoketrailemitter.h"
#include "scene/terrain.h"

extern void handleVortexCollision(Vortex *v, dBodyID pbody);
extern void handleWindVolumeCollision(WindVolume *v, dBodyID pbody);
extern QHash<dBodyID, Vortex*> g_vortices;

class World
{
public:
    World();
    virtual ~World();

    void initialize(GLFunctions *gl);
    void render(GLFunctions *gl);
    void update(float seconds);

    void setScreenSize(int width, int height);

    void keyReleaseEvent(QKeyEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseMoveDelta(float deltaX, float deltaY);

public:

    // PARTICLES
    void toggleDrawVortices();
    void toggleMovingSphere();

    // ODE stuff that is only created once per world
    dWorldID m_world_id;
    dSpaceID space;
    dMass m;
    dJointGroupID contactgroup;

    QList<ParticleEmitter*> emitters;

    ParticleEmitter *circlingEmitter;

    bool moveSphere;
    bool moveWing;

    Terrain terrain;

private:

    int m_screenWidth;
    int m_screenHeight;

    GLuint m_posAttr;
    GLuint m_texAttr;
    GLuint m_matrixUniform;
    GLuint m_textureUniform;
    GLuint m_texture;

#ifdef DEBUG_TRIANGLE
    Program m_debugFx;
#endif
};

#endif // WORLD_H
