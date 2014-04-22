#include "vortex.h"

Vortex::Vortex(float r, PxVec3 pos, PxPhysics *phys, PxScene* scene, PxMaterial *mat)
{
    PxShape* shape = phys->createShape(PxSphereGeometry(r), *mat, true);
    PxTransform localTm(pos);
    body = PxCreateKinematic(*phys, localTm, *shape, 1.0f);//phys->createRigidDynamic(localTm);
    body->attachShape(*shape);
    PxSetGroup(*body, VORTEX_GROUP);
    scene->addActor(*body);
    shape->release();

    range = r;
    active = true;

    time = 0;
}

void Vortex::draw(Obj &obj){
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_DST_COLOR);
//    glColor3f(0.8f, 0.8f, 0.5f);
//    const dReal* pos = dBodyGetPosition(body);

//    glPushMatrix();

//    glTranslatef(pos[0], pos[1], pos[2]);
//    glScalef(range*2, range*2, range*2);
//    obj.draw();

//    glPopMatrix();

//    glDisable(GL_BLEND);
}

void Vortex::update(float seconds){
    time += seconds;
    if (time > lifetime){
        active = false;
        return;
    }
    float forcefactor = exp(-forcedecay*seconds);
    float rangefactor = exp(-rangedecay*seconds);
    // TODO: right thing to do?
    // Make the force weaker, do something about the range?
    force = force*forcefactor;
    range = range/rangefactor;

    // Disable the vortex if it's too weak
    if (force < 0.001f){
        active = false;
    }
    else{
        PxTransform pos;
        body->getKinematicTarget(pos);
        body->setKinematicTarget(PxTransform(pos.p + vel*seconds));
        vel = vel*0.995f;
    }
}

//void handleVortexCollision(Vortex* v, dBodyID pbody){
//    if (v == NULL)
//        return;

//    const dReal* ppos = dBodyGetPosition(pbody);
//    const dReal* vpos = dBodyGetPosition(v->body);
//    glm::vec3 r(ppos[0] - vpos[0], ppos[1] - vpos[1], ppos[2] - vpos[2]);
//    float rlength = glm::length(r);
//    r = glm::normalize(r);

//    float force = -v->force*fmin(pow(rlength, -v->falloff), 1.0f);
//    glm::vec3 tangent = glm::cross(v->axis, r);
//    glm::vec3 f = (tangent + glm::cross(tangent, v->axis)*v->centripetal)*force;
//    dBodyAddForce(pbody, f[0], f[1], f[2]);
//}
