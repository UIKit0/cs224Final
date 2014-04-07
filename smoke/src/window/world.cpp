#include "world.h"

World::World()
{
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0, 0, 0);
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
    dMassSetSphere(&mass, 10, 0.5);
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
    for (int i = 0; i < bodies.size(); i++){
        const dReal* pos = dBodyGetPosition(bodies[i].body);
        glVertex3d(pos[0], pos[1], pos[2]);
        glVertex3d(pos[0] + bodies[i].size1, pos[1], pos[2]);
        glVertex3d(pos[0] + bodies[i].size1, pos[1], pos[2] + bodies[i].size2);
        glVertex3d(pos[0], pos[1], pos[2]);
        glVertex3d(pos[0] + bodies[i].size1, pos[1], pos[2] + bodies[i].size2);
        glVertex3d(pos[0] + bodies[i].size1, pos[1], pos[2]);
    }
    glEnd();
}

void World::tick(float seconds){
    for (int i = 0; i < bodies.size(); i++){
        dBodyAddForce(bodies[i].body, dRandReal()*2 - 1, dRandReal()*0.1, dRandReal()*2 - 1);
    }
    dWorldStep(m_world_id, 0.1f);
    for (int i = 0; i < 8; i++){
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

