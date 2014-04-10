#include "vortexshedder.h"

VortexShedder::VortexShedder(dWorldID w, dSpaceID s, dMass m, dBodyID o,
              glm::mat3x3 loc, glm::mat3x3 at,
              float vt, float vs,
              float f, float c, float r){
    owner = o;
    locationtransform = loc;
    axistransform = at;
    velThreshold = vt;
    velScale = vs;
    force = f;
    centripetal = c;
    range = r;

    world = w;
    space = s;
    mass = m;

    mainVortex = NULL;
}

void VortexShedder::destroy(){
    if (mainVortex)
        mainVortex->destroy();
}

void VortexShedder::update(float seconds){
    const dReal* v = dBodyGetLinearVel(owner);
    glm::vec3 vel(v[0], v[1], v[2]);
    float vlength = glm::length(vel);
    if (vlength > velThreshold){
        float scale = exp(velScale*(vlength - velThreshold));
        if (mainVortex == NULL)
            mainVortex = new Vortex(world, space, mass, range*scale);

//        mainVortex->falloff = 1.5f;
//        mainVortex->axis = glm::vec3(1,0,0);//axistransform * vel / vlength;
//        mainVortex->forcedecay = 0.0f;
//        mainVortex->force = 10.5f;
//        mainVortex->centripetal = centripetal;
//        mainVortex->rangedecay = 0.0f;

        mainVortex->falloff = 3.0f;
        mainVortex->force = 0.05f;
        mainVortex->centripetal = 2.5f;
        mainVortex->forcedecay = 0.5f;
        mainVortex->rangedecay = 0.05f;

//        std::cout<<force*scale<<std::endl;

        const dReal* pos = dBodyGetPosition(owner);
        glm::vec3 location = locationtransform * vel;
        dBodySetPosition(mainVortex->body, pos[0] - 2.5f,
                                    pos[1],
                                    pos[2]);
    }
    else{
        if (mainVortex)
            mainVortex->destroy();
        mainVortex = NULL;
    }

    // TODO: update everything else in the list
//    for (int i = 1; i < vortices.size(); i++){
//        vortices[i].update(seconds);
//    }
}

void VortexShedder::draw(Obj *obj){
    if (mainVortex)
        mainVortex->draw(obj);
}
