#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include <ode/ode.h>
//#include <OpenGL/glu.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include "assets/obj.h"

#define SPHERE_SIZE 2.0f

#include "vortexshedder.h"

class SolidObject
{
public:
    SolidObject();
    SolidObject(dWorldID w, dSpaceID s, dMass m);

    void update(float seconds);
    void stop();
    void draw(Obj &obj);
    void destroy();

    dBodyID body;
    dGeomID geom;

    bool movingPosX;

    QList<VortexShedder> shedders;
};

#endif // SOLIDOBJECT_H
