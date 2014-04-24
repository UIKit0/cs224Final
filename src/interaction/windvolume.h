#ifndef WINDVOLUME_H
#define WINDVOLUME_H

#include "ode/ode.h"

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <iostream>

#define WIND_VOLUME_CATEGORY_BITS 8
// Only collide with particles
#define WIND_VOLUME_COLLIDE_BITS 1

class WindVolume
{
public:
    WindVolume(dWorldID w, dGeomID g);

    dBodyID body;
    glm::vec3 force;
};

extern void handleWindVolumeCollision(WindVolume* v, dBodyID pbody);

#endif // WINDVOLUME_H
