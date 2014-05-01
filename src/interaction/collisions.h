#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "interaction/vortex.h"
#include "interaction/windvolume.h"
#include "interaction/missile.h"

#include "ode/ode.h"

extern void handleVortexCollision(Vortex* v, dBodyID pbody);
extern void handleWindVolumeCollision(WindVolume* v, dBodyID pbody);
//extern void handleBulletCollision(Bullet *b, dBodyID body);

#endif // COLLISIONS_H
