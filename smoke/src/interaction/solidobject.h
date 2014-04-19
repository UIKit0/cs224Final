#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include <ode/ode.h>
//#include <OpenGL/glu.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include "assets/obj.h"

#define SIDE_LENGTH 2.0f
#define HEIGHT 1.0f

#include "interaction/vortexshedder.h"
#include "interaction/windvolume.h"

class SolidObject
{
public:
    SolidObject();
    SolidObject(dWorldID w, dSpaceID s, dMass m);

    void update(float seconds);
    void stop();
    void start();
    void draw(Obj &obj);
    void destroy();

    dBodyID body;
    dGeomID geom;

    int moving;

    glm::vec3 direction;
    glm::vec3 left;

    QList<VortexShedder*> shedders;
    QList<WindVolume*> winds;
};

#endif // SOLIDOBJECT_H
