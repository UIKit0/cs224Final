#include "smokeparticle.h"

SmokeParticle::SmokeParticle(float radius, PxVec3 pos, PxPhysics *phys, PxScene* scene, PxMaterial *mat)
{
    PxShape* shape = phys->createShape(PxSphereGeometry(radius), *mat, true);
    PxTransform localTm(pos);
    body = phys->createRigidDynamic(localTm);
    body->attachShape(*shape);
    PxSetGroup(*body, PARTICLE_GROUP);
    scene->addActor(*body);
    shape->release();

    time = 0;
    lifetime = 0;
    active = true;

    // Random rotation
    rotation = glm::linearRand<float>(0.0,2*glm::pi<float>());

    // Start
    scale = 1.0f;
    alpha = 1.0f;
}

void SmokeParticle::draw(glm::vec3 u, glm::vec3 v, glm::vec3 z, Obj &obj){
    PxVec3 pos = body->getGlobalPose().p;

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    glScalef(scale, scale, scale);

    glRotatef(rotation*180/glm::pi<float>(), z[0], z[1], z[2]);

    glColor4f(1, 1, 1, std::min(1.0f, alpha));

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

    scale = exp(time*shrink);
    alpha = exp(time*fade);
    if (scale < minScale || alpha < minAlpha){
        active = false;
        return;
    }

    rotation += seconds*rotationSpeed;

    body->addForce(PxVec3(wind[0], wind[1], wind[2]));

    // Drag force
    body->addForce(body->getLinearVelocity()*-DAMPING_FACTOR);
}
