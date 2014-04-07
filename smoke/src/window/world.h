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

struct Particle{
    dBodyID body;
    dGeomID geom;

    Particle(dBodyID b, dGeomID g){
        body = b;
        geom = g;
    }
};

struct Vortex{
    dBodyID body;
    dGeomID geom;
    glm::vec3 axis;
    float force;
    float falloff;
    float range;

    Vortex(dBodyID b, dGeomID g){
        body = b;
        geom = g;
    }
};

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

    dWorldID m_world_id;
    dSpaceID space;
    dGeomID geom;
    dMass m;

    dJointGroupID contactgroup;

    QList<Particle> particles;

    QList<Vortex> vortices;

    // Sphere that moves left and right through the smoke
    dBodyID object;
    dGeomID obj;
    dVector3 objVel;
    bool movingPosX;
};

#endif // WORLD_H
