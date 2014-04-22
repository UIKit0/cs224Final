#include "world.h"

World::World() :
    sphereMesh("cube.obj")
{
    // Initialize ODE stuff
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0, 0, 0);

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

QOpenGLShaderProgram* World::m_shader = 0;

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
            handleVortexCollision((Vortex*)dBodyGetData(b1), b2);
        }
        else if (dGeomGetCategoryBits(o2) == VORTEX_CATEGORY_BITS){
            handleVortexCollision((Vortex*)dBodyGetData(b2), b1);
        }
        else if (dGeomGetCategoryBits(o1) == WIND_VOLUME_CATEGORY_BITS){
            handleWindVolumeCollision((WindVolume*)dBodyGetData(b1), b2);
        }
        else if (dGeomGetCategoryBits(o2) == WIND_VOLUME_CATEGORY_BITS){
            handleWindVolumeCollision((WindVolume*)dBodyGetData(b2), b1);
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

    BasicSmokeEmitter *emitter = new BasicSmokeEmitter(m_world_id, space, m);
    emitter->maxInitialVel = glm::vec3(0.5f, 2.0f, 0.5f);
    emitter->minInitialVel = glm::vec3(-0.5f, 0.5f, -0.5f);
    emitters.append(emitter);

    circlingEmitter = new SmokeTrailEmitter(m_world_id, space, m);
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
    if (1/seconds < 40)
        std::cout<<"fps: "<<(1/seconds)<<std::endl;
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

void World::setShader(QOpenGLShaderProgram *s)
{
    m_shader = s;
}
