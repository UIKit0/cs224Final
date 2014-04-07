#include "world.h"

World::World()
{
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0, 0, 0);
    dWorldSetLinearDamping(m_world_id, 0.05f);
    for (int i = 0; i < 10; i++){
        addBody();
    }
    dAllocateODEDataForThread(dAllocateMaskAll);
}

World::~World(){
    dWorldDestroy(m_world_id);
    dCloseODE();
}

void World::addBody(){
    dBodyID body = dBodyCreate(m_world_id);
    dBodySetPosition(body, dRandReal()*10 - 10,
                     dRandReal(),
                     dRandReal()*10 - 10);
    dBodySetLinearVel(body, dRandReal()*2 - 1, dRandReal(), dRandReal()*2 - 1);
    dMass mass;
    dMassSetSphere(&mass, 10, 0.5f);
    dBodySetMass(body, &mass);
    bodies.append(Particle(body, dRandReal()*2, dRandReal()*2));
}

void World::init()
{
    initializeOpenGLFunctions();
}

void World::draw()
{
    glColor3f(1,0,0);
    glBegin(GL_TRIANGLES);
    GLUquadricObj* quad = gluNewQuadric();

    for (int i = 0; i < bodies.size(); i++){
        const dReal* pos = dBodyGetPosition(bodies[i].body);
        glPushMatrix();
        glTranslatef(pos[0], pos[1], pos[2]);
        gluSphere(quad, bodies[i].size1, 32, 32);
        glPopMatrix();
    }
    glEnd();
}

void World::tick(float seconds){
    // Upwards force
    for (int i = 0; i < bodies.size(); i++){
        dBodyAddForce(bodies[i].body, dRandReal()*2 - 1, dRandReal()*3, dRandReal()*2 - 1);
    }
    dWorldStep(m_world_id, 0.1f);
    for (int i = 0; i < 1; i++){
        addBody();
    }

    for (int i = bodies.size() - 1; i >= 0; i--){
        const dReal *pos = dBodyGetPosition(bodies[i].body);
        if (pos[1] > 50){
            dBodyDestroy(bodies[i].body);
            bodies.removeAt(i);
        }
    }
    std::cout<<bodies.size()<<std::endl;
}

