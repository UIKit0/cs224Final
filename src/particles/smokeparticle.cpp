#include "smokeparticle.h"

SmokeParticle::SmokeParticle(dWorldID world, dSpaceID space, dMass mass, float radius)
{
    this->mass = mass;
    this->size = radius;
    body = dBodyCreate(world);
    geom = dCreateSphere(space, radius);

    dGeomSetBody(geom, body);

    // Collide with everything except self
    dGeomSetCategoryBits(geom, PARTICLE_CATEGORY_BITS);
    dGeomSetCollideBits(geom, ~PARTICLE_CATEGORY_BITS);

    // Unit mass
    dMassSetSphereTotal(&mass, 1.0f, radius);
    dBodySetMass(body, &mass);

    time = 0;
    lifetime = 0;
    expandtime = 0;
    active = true;

    // Random rotation
    rotation = dRandReal()*M_PI*2;

    // Start
    initialScale = 1.0f;
    scale = 1.0f;
    alpha = 1.0f;
}

void SmokeParticle::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

glm::vec3 SmokeParticle::getPosition(){
    const dReal* pos = dBodyGetPosition(body);
    return glm::vec3(pos[0], pos[1], pos[2]);
}

void SmokeParticle::update(float seconds){
    time += seconds;

    scale = initialScale*exp(time*expand);
//    std::cout<<scale<<std::endl;
    if (scale > 1.0f && expandtime == 0){
        expandtime = time;
    }
    if (expandtime != 0 && time > expandtime){
        scale = exp((time - expandtime)*shrink);
    }

    alpha = exp(time*fade);
    if (scale < minScale || alpha < minAlpha || time > lifetime){
        active = false;
        return;
    }

    // TODO: update the collision radius

    rotation += seconds*rotationSpeed;

    dBodyAddForce(body, wind[0]*dRandReal(), wind[1]*dRandReal(), wind[2]*dRandReal());

    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*DAMPING_FACTOR, -vel[1]*DAMPING_FACTOR, -vel[2]*DAMPING_FACTOR);
}
