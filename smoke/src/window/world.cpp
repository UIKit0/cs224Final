#include "world.h"

//#define SPHERE_SIZE 2.0f


// TODO: move to a smoke emitter class


World::World()
{
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0, 0, 0);

    // TODO: make this a quad structure maybe? or hash?
    space = dSimpleSpaceCreate(0);

    quad = gluNewQuadric();

    emitters.append(new SmokeEmitter(m_world_id, space, m));

    sphere = SolidObject(m_world_id, space, m);

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
Vortex World::lookupVortex(dBodyID v){
    for (int i = 0; i < emitters.size(); i++){
        for (int j = 0; j < emitters[i]->vortices.size(); j++){
            if (emitters[i]->vortices[j].body == v){
                return emitters[i]->vortices[j];
            }
        }
    }
}

static void handleVortexCollision(Vortex v, dBodyID pbody){
    const dReal* ppos = dBodyGetPosition(pbody);
    const dReal* vpos = dBodyGetPosition(v.body);
    glm::vec3 r(ppos[0] - vpos[0], ppos[1] - vpos[1], ppos[2] - vpos[2]);
    float rlength = glm::length(r);
    r = glm::normalize(r);

    float force = -v.force*fmin(pow(rlength, -v.falloff), 1.0f);
    glm::vec3 tangent = glm::cross(v.axis, r);
    glm::vec3 f = (tangent + glm::cross(tangent, v.axis)*v.centripetal)*force;
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
}

void World::draw()
{
    glPushMatrix();
    glScalef(0.3f, 0.3f, 0.3f);

    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->draw(quad);
    }

    // Sphere
    sphere.draw(quad);

    glPopMatrix();

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
}

void World::tick(float seconds){
    // Upwards force
    dSpaceCollide(space, this, nearCallback);
    sphere.update(seconds);

    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->update(seconds);
    }

    dWorldQuickStep(m_world_id, 0.35f);
    dJointGroupEmpty(contactgroup);
}

