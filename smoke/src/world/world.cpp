#include "world.h"

World::World() :
    sphereMesh("cube.obj")
{
    // Initialize ODE stuff
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0, 0, 0);

    perlin = new PerlinNoise(0.2f, 5);

    dVector3 center;
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    dVector3 extent;
    extent[0] = 1;
    extent[1] = 1;
    extent[2] = 1;
    space = dHashSpaceCreate(0);
    dHashSpaceSetLevels(space, 1, 5);

    contactgroup = dJointGroupCreate(0);

    dAllocateODEDataForThread(dAllocateMaskAll);

    moveSphere = false;
    moveWing = false;
}

World::~World(){
    emitters.clear();
    dJointGroupDestroy(contactgroup);
    dSpaceDestroy(space);
    dWorldDestroy(m_world_id);
    dCloseODE();
}

void World::toggleDrawVortices(){
    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->drawVortices = !emitters[i]->drawVortices;
    }
}

void World::toggleMovingSphere(){
    if (sphere.moving != 0)
        sphere.stop();
    else
        sphere.start();
}

// TODO: make this runtime not horrendous
Vortex* World::lookupVortex(dBodyID v){
    if (g_vortices.contains(v))
        return g_vortices.value(v);
    std::cout<<"no"<<std::endl;
    return NULL;
}

static void handleVortexCollision(Vortex* v, dBodyID pbody){
    if (v == NULL)
        return;

    const dReal* ppos = dBodyGetPosition(pbody);
    const dReal* vpos = dBodyGetPosition(v->body);
    glm::vec3 r(ppos[0] - vpos[0], ppos[1] - vpos[1], ppos[2] - vpos[2]);
    float rlength = glm::length(r);
    r = glm::normalize(r);

    float force = -v->force*fmin(pow(rlength, -v->falloff), 1.0f);
    glm::vec3 tangent = glm::cross(v->axis, r);
    glm::vec3 f = (tangent + glm::cross(tangent, v->axis)*v->centripetal)*force;
    dBodyAddForce(pbody, f[0], f[1], f[2]);
}

static void nearCallback(void* data, dGeomID o1, dGeomID o2){
    World* world = (World*) data;
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);

    dContact contact;
    contact.surface.mode = dContactBounce | dContactSoftCFM;
    contact.surface.mu = dInfinity;
    contact.surface.bounce = 0.01f;
    contact.surface.bounce_vel = 0.1f;
    contact.surface.soft_cfm = 0.001;
    if (dCollide(o1, o2, 1, &contact.geom, sizeof(dContact))){
        if (dGeomGetCategoryBits(o1) == VORTEX_CATEGORY_BITS){
            handleVortexCollision(world->lookupVortex(b1), b2);
        }
        else if (dGeomGetCategoryBits(o2) == VORTEX_CATEGORY_BITS){
            handleVortexCollision(world->lookupVortex(b2), b1);
        }
        else{
            dJointID c = dJointCreateContact(world->m_world_id, world->contactgroup, &contact);
            dJointAttach(c, b1, b2);
        }
    }
}

void World::init()
{
    initializeOpenGLFunctions();

    GLfloat LightAmbient[]=  { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat LightDiffuse[]=  { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

    glEnable(GL_TEXTURE_2D);

    SmokeEmitter *emitter = new SmokeEmitter(m_world_id, space, m);
    emitter->maxInitialVel = glm::vec3(0.5f, 2.0f, 0.5f);
    emitter->minInitialVel = glm::vec3(-0.5f, 0.5f, -0.5f);
    emitters.append(emitter);

    circlingEmitter = new SmokeEmitter(m_world_id, space, m);
    circlingEmitter->location = glm::vec3(30,0,0);

    sphere = SolidObject(m_world_id, space, m);
}

void World::draw()
{
    // Draw grid
    glColor4f(0, 0, 0, 0.25);
    glBegin(GL_LINES);
    for (int s = 20, i = -s; i <= s; i++) {
        glVertex3f(i, 0, -s);
        glVertex3f(i, 0, +s);
        glVertex3f(-s, 0, i);
        glVertex3f(+s, 0, i);
    }
    glEnd();

    // Sphere
    sphere.draw(sphereMesh);

    circlingEmitter->draw(sphereMesh);

    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->draw(sphereMesh);
    }
}

void World::tick(float seconds){
    // Upwards force
    dSpaceCollide(space, this, nearCallback);
    sphere.update(seconds);

    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->update(seconds);
    }
    float angle = atan2(circlingEmitter->location[2], circlingEmitter->location[0]);
    if (angle < 0)
        angle = M_PI*2 + angle;

    circlingEmitter->location = glm::vec3(30*cos(angle + 0.02f), 0, 30*sin(angle + 0.02f));
    circlingEmitter->update(seconds);

    dWorldQuickStep(m_world_id, 1/30.0f);
    dJointGroupEmpty(contactgroup);
}

