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

#include <ode/ode.h>

#include <iostream>

class World : protected QOpenGLFunctions
{
public:
    World();
    virtual ~World();

    void init();
    void draw();
    void tick(float seconds);

private:
    void addBody();

     dWorldID m_world_id;
     dSpaceID space;
     dBodyID body;
     dGeomID geom;
     dMass m;

    QList<dBodyID> bodies;
};

#endif // WORLD_H
