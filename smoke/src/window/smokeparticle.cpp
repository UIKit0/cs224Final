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

void ODEtoOGL(float* M, const float* pos, const float* R)
{
    M[0]  = R[0]; M[1]  = R[4]; M[2]  = R[8];  M[3]  = 0;

    M[4]  = R[1]; M[5]  = R[5]; M[6]  = R[9];  M[7]  = 0;

    M[8]  = R[2]; M[9]  = R[6]; M[10] = R[10]; M[11] = 0;

    M[12] = pos[0]; M[13] = pos[1]; M[14] = pos[2];  M[15] = 1;

}

void SmokeParticle::draw(glm::vec3 u, glm::vec3 v, glm::vec3 z, Obj &obj){
    const dReal* pos = dBodyGetPosition(body);
    const dReal* orient = dBodyGetRotation(body);

    glTranslatef(pos[0], pos[1], pos[2]);

    dReal scale = dGeomSphereGetRadius(geom)/PARTICLE_SIZE;
    glScalef(scale, scale, scale);

    glRotatef(rotation*180/M_PI, z[0], z[1], z[2]);

//    obj.draw();

    glm::vec3 corner = u + v;
    glm::vec3 corner2 = v - u;
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(corner[0], corner[1], corner[2]);
    glTexCoord2f(0, 1);
    glVertex3f(corner2[0], corner2[1], corner2[2]);
    glTexCoord2f(1, 1);
    glVertex3f(-corner[0], -corner[1], -corner[2]);
    glTexCoord2f(1, 0);
    glVertex3f(-corner2[0], -corner2[1], -corner2[2]);
    glEnd();

    glRotatef(-rotation*180/M_PI, z[0], z[1], z[2]);
    glScalef(1/scale, 1/scale, 1/scale);

    glTranslatef(-pos[0], -pos[1], -pos[2]);
}

void SmokeParticle::update(float seconds){
    time += seconds;
    lifetime += seconds;

    float decay = pow(0.85f, lifetime);
    if (decay < 0.2f){
        active = false;
        return;
    }

    rotation += seconds/M_PI;

    dGeomSphereSetRadius(geom, PARTICLE_SIZE*decay);

    // TODO: change the mass without everything blowing up?
//    dMassSetSphere(&mass, 1/PARTICLE_SIZE, PARTICLE_SIZE*decay);
//    dBodySetMass(body, &mass);

    float scale = 0.2f;
    dBodyAddForce(body, 0, dRandReal()*scale, 0);
    const dReal *loc = dBodyGetPosition(body);

    float noise = perlin->perlin_noise(loc[1], time*scale, loc[0]*loc[0] + loc[2]*loc[2]);
    float noise2 = perlin->perlin_noise(loc[1] + 1, time*scale, loc[0]*loc[0] + loc[2]*loc[2]);
    dBodyAddForce(body, noise*scale*10, 0, noise2*scale*10);

    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*DAMPING_FACTOR, -vel[1]*DAMPING_FACTOR, -vel[2]*DAMPING_FACTOR);
}
