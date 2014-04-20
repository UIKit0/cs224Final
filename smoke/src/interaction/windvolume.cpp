#include "windvolume.h"

WindVolume::WindVolume(dWorldID w, dGeomID g)
{
    body = dBodyCreate(w);
    dGeomSetBody(g, body);
    dGeomSetCategoryBits(g, WIND_VOLUME_CATEGORY_BITS);
    dGeomSetCollideBits(g, WIND_VOLUME_COLLIDE_BITS);
    dBodySetKinematic(body);
    dBodySetData(body, this);
}

void handleWindVolumeCollision(WindVolume* v, dBodyID pbody){
    if (v == NULL)
        return;
    dBodyAddForce(pbody, v->force[0], v->force[1], v->force[2]);
}
