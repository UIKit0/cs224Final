#ifndef VORTEX_H
#define VORTEX_H

#include <ode/ode.h>
//#include <OpenGL/glu.h>
#include <qgl.h>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QHash>

#include <iostream>
<<<<<<< HEAD

#define VORTEX_CATEGORY_BITS 2
// Only collide with particles
#define VORTEX_COLLIDE_BITS 1
=======
#include "assets/obj.h"
#define VORTEX_CATEGORY_BITS 7
>>>>>>> 2cf21ad35219b15dffa73b4f434ab4bab349f200

class Vortex
{
public:
    Vortex(dWorldID world, dSpaceID space, dMass mass, float r);

    void update(float seconds);
    void draw(Obj &obj);
    void destroy();

    dBodyID body;
    dGeomID geom;

    bool active;

    // Vortex parameters
    glm::vec3 axis;
    float force;
    float falloff;
    float range;
    float centripetal;
    float forcedecay;
    float rangedecay;

    static QHash<dBodyID, Vortex*> g_vortices;
};

#endif // VORTEX_H
