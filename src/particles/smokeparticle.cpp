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
    active = true;

    // Random rotation
    rotation = dRandReal()*M_PI*2;

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

    g_model.pushMatrix();

    g_model.matrix = glm::translate(g_model.matrix, glm::vec3(pos[0], pos[1], pos[2]));
    g_model.matrix = glm::scale(g_model.matrix, glm::vec3(scale, scale, scale));
    g_model.matrix = glm::rotate(g_model.matrix, (float)(rotation), glm::vec3(z[0], z[1], z[2]));

    glColor4f(0.5,0.5,0.5, fmin(1.0f, alpha));

    glMultMatrixf(glm::value_ptr(g_model.matrix));

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
    g_model.popMatrix();
}

void SmokeParticle::update(float seconds){
    time += seconds;

    scale = exp(time*shrink);
    alpha = exp(time*fade);
    if (scale < minScale || alpha < minAlpha){
        active = false;
        return;
    }

    rotation += seconds*rotationSpeed;

    dBodyAddForce(body, wind[0]*dRandReal(), wind[1]*dRandReal(), wind[2]*dRandReal());

    // Drag force
    const dReal* velocity = dBodyGetLinearVel(body);
    glm::vec3 vel(velocity[0], velocity[1], velocity[2]);
    dBodyAddForce(body, -vel[0]*DAMPING_FACTOR, -vel[1]*DAMPING_FACTOR, -vel[2]*DAMPING_FACTOR);
}
