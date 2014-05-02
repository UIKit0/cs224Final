#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "interaction/vortex.h"
#include "interaction/windvolume.h"
//#include "interaction/missile.h"
//#include "interaction/enemy.h"

#include "ode/ode.h"

extern void handleVortexCollision(Vortex* v, dBodyID pbody);
extern void handleWindVolumeCollision(WindVolume* v, dBodyID pbody);
//extern void handleMissileCollision(Missile *m, dBodyID body);
//extern void handleMissileEnemyCollision(Missile *m, Enemy *e);

#endif // COLLISIONS_H
