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
#include "scene/contour.h"
#include "scene/player.h"

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
    dJointGroupID contactgroup;
    dWorldID m_world_id;

private:
    Terrain m_terrain;
    Contour m_contour;

    // ODE stuff that is only created once per world
    dSpaceID space;
    dMass m;

    // PARTICLES
    QList<ParticleEmitter*> emitters;
    ParticleEmitter *circlingEmitter;
    // Sphere that moves left and right through the smoke
    void toggleMovingSphere();
    void toggleDrawVortices();
    SolidObject sphere;
    bool moveSphere;
    bool moveWing;

    Player player;

    Terrain terrain;

    int m_screenWidth;
    int m_screenHeight;

#ifdef DEBUG_TRIANGLE
    Program m_debugFx;
#endif
};

#endif // WORLD_H
