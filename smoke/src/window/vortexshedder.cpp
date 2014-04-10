#include "vortexshedder.h"

VortexShedder::VortexShedder(dWorldID w, dSpaceID s, dMass m, dBodyID o,
              glm::vec3 loc, glm::mat3x3 at,
              float vt, float vs,
              float f, float r){
    owner = o;
    location = loc;
    mass = m;
    axistransform = at;
    velThreshold = vt;
    velScale = vs;
    force = f;
    range = r;

    world = w;
    space = s;

    Vortex v = Vortex(w, s, m, r);
    vortices.append(v);
}

void VortexShedder::update(float seconds){
    const dReal* v = dBodyGetLinearVel(owner);
    glm::vec3 vel(v[0], v[1], v[2]);
    float vlength = vel.length();
    if (vlength > velThreshold){
        float scale = exp(velScale*(vlength - velThreshold));
        Vortex newv = Vortex(world, space, mass, range*scale);
        newv.falloff = 1.5f;
        newv.axis = axistransform * vel / vlength;
        newv.forcedecay = 0.0f;
        newv.force = force*scale;

        const dReal* pos = dBodyGetPosition(owner);
        dBodySetPosition(newv.body, pos[0] + location[0],
                                    pos[1] + location[1],
                                    pos[2] + location[2]);

        // Remove old vortex and replace it with the new one
        vortices[0].destroy();
        vortices.replace(0, newv);
    }

    // TODO: update everything else in the list
    for (int i = 1; i < vortices.size(); i++){
        vortices[i].update(seconds);
    }
}

void VortexShedder::draw(Obj &obj){
    vortices[0].draw(obj);
}
