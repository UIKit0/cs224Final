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
<<<<<<< HEAD
    void start();
    void draw(GLUquadric *quad);
=======
    void draw(Obj &obj);
>>>>>>> 2cf21ad35219b15dffa73b4f434ab4bab349f200
    void destroy();

    dBodyID body;
    dGeomID geom;

    int moving;

    QList<VortexShedder> shedders;
};

#endif // SOLIDOBJECT_H
