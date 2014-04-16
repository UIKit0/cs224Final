#include "smokeparticle.h"

SmokeParticle::SmokeParticle(dWorldID world, dSpaceID space, dMass mass, PerlinNoise *perlin, float radius)
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

    this->perlin = perlin;
    type = (int)(dRandReal() * 4);
    time = 0;
    lifetime = 0;
    active = true;

    // Random rotation
    rotation = dRandReal()*M_PI*2;
    if (dRandReal() < 0.5f)
        rotDirection = 1;
    else
        rotDirection = -1;

    // Start
    scale = 1.0f;
    alpha = 1.0f;
}

void SmokeParticle::destroy(){
    dGeomDestroy(geom);
    dBodyDestroy(body);
}

void SmokeParticle::draw(glm::vec3 u, glm::vec3 v, glm::vec3 z, Obj &obj){
    const dReal* pos = dBodyGetPosition(body);

    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);

    glScalef(scale, scale, scale);

    glRotatef(rotation*180/M_PI, z[0], z[1], z[2]);

    glColor4f(1, 1, 1, fmin(1.0f, alpha));

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

    glPopMatrix();
}

void SmokeParticle::update(float seconds){
    time += seconds;
    lifetime += seconds;

    scale = exp(lifetime*shrink);
    alpha = exp(lifetime*fade);
    if (scale < minScale || alpha < minAlpha){
        active = false;
        return;
    }

    rotation += seconds/M_PI*rotDirection;

    dGeomSphereSetRadius(geom, size*scale);

    // TODO: change the mass without everything blowing up?
//    dMassSetSphere(&mass, 1/PARTICLE_SIZE, PARTICLE_SIZE*decay);
//    dBodySetMass(body, &mass);

    float timescale = 0.2f;
    dBodyAddForce(body, 0, dRandReal(), 0);
    const dReal *loc = dBodyGetPosition(body);

    float noise = perlin->perlin_noise(loc[1], time*timescale, loc[0]*loc[0] + loc[2]*loc[2]);
    float noise2 = perlin->perlin_noise(loc[1] + 1, time*timescale, loc[0]*loc[0] + loc[2]*loc[2]);
    dBodyAddForce(body, noise*3, 0, noise2*3);

    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*DAMPING_FACTOR, -vel[1]*DAMPING_FACTOR, -vel[2]*DAMPING_FACTOR);
}
