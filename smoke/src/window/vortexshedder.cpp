#include "vortexshedder.h"

VortexShedder::VortexShedder(dWorldID w, dSpaceID s, dMass m, dBodyID o) : time(0)
{
    owner = o;
    space = s;
    world = w;
    mass = m;
}

void VortexShedder::destroy(){
    vortices.clear();
}

void VortexShedder::update(float seconds){
    time += seconds;
    const dReal* v = dBodyGetLinearVel(owner);
    glm::vec3 vel(v[0], v[1], v[2]);
    float vlength = glm::length(vel);
    if (vlength > velThreshold && time > range*2/vlength){
        time = 0;
        float scale = exp(velScale*(vlength - velThreshold));
        Vortex* newVortex = new Vortex(world, space, mass, range);

        newVortex->axis = axis;
        newVortex->falloff = falloff;
        newVortex->force = force;
        newVortex->forcedecay = forcedecay;
        newVortex->centripetal = centripetal;
        newVortex->rangedecay = 0.05f;

        dBodySetPosition(newVortex->body, location[0],
                                    location[1],
                                    location[2]);

        dBodySetLinearVel(newVortex->body, v[0], v[1], v[2]);
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
