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
#include <GL/glu.h>

#include <ode/ode.h>
#include <GL/glu.h>

#include <iostream>

#include "smokeparticle.h"
#include "vortex.h"
#include "solidobject.h"

class World : protected QOpenGLFunctions
{
public:
    World();
    virtual ~World();

    void init();
    void draw();
    void tick(float seconds);

    GLUquadric *quad;

    void addBody();
    void addVortex();

    // Values that are only created once per world
    dWorldID m_world_id;
    dSpaceID space;
    dMass m;

    dJointGroupID contactgroup;

    QList<SmokeParticle> particles;

    QList<Vortex> vortices;

    // Sphere that moves left and right through the smoke
    SolidObject sphere;

    bool moveSphere;
    bool drawVortices;
    bool moveWing;
};

#endif // WORLD_H
