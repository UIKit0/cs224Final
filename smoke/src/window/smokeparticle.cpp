#include "smokeparticle.h"

SmokeParticle::SmokeParticle(dWorldID world, dSpaceID space, dMass mass, PerlinNoise *perlin)
{
    this->mass = mass;
    body = dBodyCreate(world);
    geom = dCreateSphere(space, PARTICLE_SIZE);

    dGeomSetBody(geom, body);

    // Collide with everything except self
    dGeomSetCategoryBits(geom, PARTICLE_CATEGORY_BITS);
    dGeomSetCollideBits(geom, ~PARTICLE_CATEGORY_BITS);

    // Unit mass
    dMassSetSphere(&mass, 1/PARTICLE_SIZE, PARTICLE_SIZE);
    dBodySetMass(body, &mass);

    this->perlin = perlin;
    type = (int)(dRandReal() * 4);
    time = 0;
    lifetime = 0;
    active = true;
}

void SmokeParticle::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void SmokeParticle::draw(glm::vec3 u, glm::vec3 v, Obj &obj){
    const dReal* pos = dBodyGetPosition(body);

    glTranslatef(pos[0], pos[1], pos[2]);

//    float decay = pow(0.85f, lifetime);
    dReal scale = dGeomSphereGetRadius(geom);
    glScalef(scale, scale, scale);
    obj.draw();
    glScalef(1/scale, 1/scale, 1/scale);

//    glm::vec3 corner = u + v;
//    glm::vec3 corner2 = v - u;
//    glBegin(GL_QUADS);
//    glTexCoord2f(0, 0);
//    glVertex3f(corner[0], corner[1], corner[2]);
//    glTexCoord2f(0, 0.25f);
//    glVertex3f(corner2[0], corner2[1], corner2[2]);
//    glTexCoord2f(0.25f, 0.25f);
//    glVertex3f(-corner[0], -corner[1], -corner[2]);
//    glTexCoord2f(0.25f, 0);
//    glVertex3f(-corner2[0], -corner2[1], -corner2[2]);
//    glEnd();

    glTranslatef(-pos[0], -pos[1], -pos[2]);
}

void SmokeParticle::update(float seconds){
    time += seconds;
    lifetime += seconds;

    float decay = pow(0.85f, lifetime);
    if (decay < 0.1f){
        active = false;
        return;
    }

    dGeomSphereSetRadius(geom, PARTICLE_SIZE*decay);

    // TODO: change the mass without everything blowing up?
//    dMassSetSphere(&mass, 1/PARTICLE_SIZE, PARTICLE_SIZE*decay);
//    dBodySetMass(body, &mass);

    float scale = 1.0f;//*decay;
    dBodyAddForce(body, 0, dRandReal()*scale, 0);
    const dReal *loc = dBodyGetPosition(body);

    float noise = perlin->perlin_noise(loc[1], time*scale*10, loc[0]*loc[0] + loc[2]*loc[2]);
    float noise2 = perlin->perlin_noise(loc[1] + 1, time*scale*10, loc[0]*loc[0] + loc[2]*loc[2]);
    dBodyAddForce(body, noise*scale, 0, noise2*scale);

    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*DAMPING_FACTOR, -vel[1]*DAMPING_FACTOR, -vel[2]*DAMPING_FACTOR);
}
