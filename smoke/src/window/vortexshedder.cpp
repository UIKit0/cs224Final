#include "vortexshedder.h"

VortexShedder::VortexShedder(dWorldID w, dSpaceID s, dMass m, dBodyID o){
    owner = o;
    world = w;
    space = s;
    mass = m;
}

void VortexShedder::destroy(){
    vortices.clear();
}

void VortexShedder::update(float seconds){
    const dReal* v = dBodyGetLinearVel(owner);
    glm::vec3 vel(v[0], v[1], v[2]);
    float vlength = glm::length(vel);
    if (vlength > velThreshold && dRandReal() > 0.9f){
        float scale = exp(velScale*(vlength - velThreshold));
        Vortex* newVortex = new Vortex(world, space, mass, range);

        newVortex->axis = axis;
        newVortex->falloff = falloff;
        newVortex->force = force;
        newVortex->forcedecay = forcedecay;
        newVortex->centripetal = centripetal;
        newVortex->rangedecay = 0.05f;

        const dReal* pos = dBodyGetPosition(owner);
        dBodySetPosition(newVortex->body, location[0],
                                    location[1],
                                    location[2]);

        const dReal* vel = dBodyGetLinearVel(owner);
        dBodySetLinearVel(newVortex->body, vel[0], vel[1], vel[2]);
        vortices.append(newVortex);
    }

    for (int i = vortices.size() - 1; i >= 0; i--){
        vortices[i]->update(seconds);
        if (!vortices[i]->active){
            delete vortices[i];
            vortices.removeAt(i);
        }
    }
}

void VortexShedder::draw(Obj &obj){
    for (int i = 1; i < vortices.size(); i++){
        vortices[i]->draw(obj);
    }
}
