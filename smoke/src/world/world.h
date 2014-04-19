#ifndef WORLD_H
#define WORLD_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/noise.hpp>

#include <QHash>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QList>
#include <QGLContext>

#include "assets/obj.h"

#include <ode/ode.h>

#include <iostream>

#include "particles/smokeparticle.h"
#include "interaction/vortex.h"
#include "interaction/solidobject.h"
#include "particles/basicsmokeemitter.h"
#include "particles/smoketrailemitter.h"
#include "world/perlinnoise.h"

extern void handleVortexCollision(Vortex *v, dBodyID pbody);
extern void handleWindVolumeCollision(WindVolume *v, dBodyID pbody);
extern QHash<dBodyID, Vortex*> g_vortices;

class World : protected QOpenGLFunctions
{
public:
    World();
    virtual ~World();

    void init();
    void draw();
    void tick(float seconds);

    void toggleDrawVortices();
    void toggleMovingSphere();

    // Values that are only created once per world
    dWorldID m_world_id;
    dSpaceID space;
    dMass m;

    PerlinNoise *perlin;

    dJointGroupID contactgroup;

    QList<ParticleEmitter*> emitters;

    ParticleEmitter *circlingEmitter;

    // Sphere that moves left and right through the smoke
    SolidObject sphere;
    Obj sphereMesh;

    bool moveSphere;
    bool moveWing;
};

#endif // WORLD_H
