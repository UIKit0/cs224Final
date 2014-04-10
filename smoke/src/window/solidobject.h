#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include <ode/ode.h>
#include <GL/glu.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#define SPHERE_SIZE 2.0f

#include "vortexshedder.h"

class SolidObject
{
public:
    SolidObject();
    SolidObject(dWorldID w, dSpaceID s, dMass m);

    void update(float seconds);
    void stop();
    void start();
    void draw(GLUquadric *quad);
    void destroy();

    dBodyID body;
    dGeomID geom;

    int moving;

    QList<VortexShedder> shedders;
};

#endif // SOLIDOBJECT_H
