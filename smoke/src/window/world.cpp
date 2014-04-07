#include "world.h"

World::World()
{
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0.0f, -0.1f, 0.0f);
    addBody();
    addBody();
    dAllocateODEDataForThread(dAllocateMaskAll);
}

World::~World(){
    dWorldDestroy(m_world_id);
    dCloseODE();
}

void World::addBody(){
    dBodyID body = dBodyCreate(m_world_id);
    dBodySetPosition(body, rand()/(double)RAND_MAX*10 - 5,
                            rand()/(double)RAND_MAX*10,
                            rand()/(double)RAND_MAX*10 - 5);
    dBodySetLinearVel(body, 0, 0, 0);
    bodies.append(body);
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
        const dReal* pos = dBodyGetPosition(bodies[i]);
        glVertex3d(pos[0], pos[1], pos[2]);
        glVertex3d(pos[0] + 1, pos[1], pos[2]);
        glVertex3d(pos[0] + 1, pos[1], pos[2] + 1);
        glVertex3d(pos[0], pos[1], pos[2]);
        glVertex3d(pos[0] + 1, pos[1], pos[2] + 1);
        glVertex3d(pos[0] + 1, pos[1], pos[2]);
    }
    glEnd();
}

void World::tick(float seconds){
    dWorldStep(m_world_id, 0.1f);
}

