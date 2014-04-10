#include "world.h"

//#define SPHERE_SIZE 2.0f


// TODO: move to a smoke emitter class
#define SPAWN_SIZE 3
#define MAX_HEIGHT 20

World::World() :
    sphereMesh("cube.obj")
{
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0, 0, 0);

    // TODO: make this a quad structure maybe? or hash?
    space = dSimpleSpaceCreate(0);

    for (int i = 0; i < 10; i++){
        addBody();
    }

    addVortex();

//    object = dBodyCreate(m_world_id);
//    dBodySetKinematic(object);
//    dMassSetSphere(&m, 10.0f, SPHERE_SIZE);
//    obj = dCreateSphere(space, SPHERE_SIZE);
//    dGeomSetBody(obj, object);
//    dBodySetPosition(object, 10, 10, 0);
//    movingPosX = false;
    sphere = SolidObject(m_world_id, space, m);

    contactgroup = dJointGroupCreate(0);

    dAllocateODEDataForThread(dAllocateMaskAll);

    drawVortices = false;
    moveSphere = false;
    moveWing = false;
}

World::~World(){
    dJointGroupDestroy(contactgroup);
    dSpaceDestroy(space);
    dWorldDestroy(m_world_id);
    dCloseODE();
}

static void handleVortexCollision(World *world, dBodyID vBody, dBodyID pbody){
    for (int i = 0; i < world->vortices.size(); i++){
        if (world->vortices[i].body == vBody){
            Vortex v = world->vortices[i];
            const dReal* ppos = dBodyGetPosition(pbody);
            const dReal* vpos = dBodyGetPosition(vBody);
            glm::vec3 r(ppos[0] - vpos[0], ppos[1] - vpos[1], ppos[2] - vpos[2]);
            float rlength = r.length();

            float force = -v.force*fmin(pow(rlength, -v.falloff), 1.0f);
            glm::vec3 tangent = glm::cross(v.axis, r/rlength);
            glm::vec3 f = (tangent + glm::cross(tangent, v.axis)*0.5f)*force;
            dBodyAddForce(pbody, f[0], f[1], f[2]);

            return;
        }
    }
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
            handleVortexCollision(world, b1, b2);
        }
        else if (dGeomGetCategoryBits(o2) == VORTEX_CATEGORY_BITS){
            handleVortexCollision(world, b2, b1);
        }
        else{
            dJointID c = dJointCreateContact(world->m_world_id, world->contactgroup, &contact);
            dJointAttach(c, b1, b2);
        }
    }
}

void World::addBody(){
    SmokeParticle sp = SmokeParticle(m_world_id, space, m);

    dBodySetPosition(sp.body, dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2,
                     dRandReal(),
                     dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2);
    float maxInitialVel = 0.05f;
    float maxVerticalVel = 0.3f;
    dBodySetLinearVel(sp.body, dRandReal()*maxInitialVel*2 - maxInitialVel,
                            dRandReal()*maxVerticalVel,
                            dRandReal()*maxInitialVel*2 - maxInitialVel);

    particles.append(sp);
}

void World::addVortex(){
    Vortex v = Vortex(m_world_id, space, m, dRandReal()*2 + 0.5f);

    dBodySetPosition(v.body, dRandReal()*SPAWN_SIZE*2 - SPAWN_SIZE, 3, dRandReal()*SPAWN_SIZE*2 - SPAWN_SIZE);

    v.axis = glm::normalize(glm::vec3(dRandReal(), dRandReal(), dRandReal()));
    v.falloff = 2.0f;
    v.force = dRandReal()*0.5f;
    v.forcedecay = 0.5f;
    v.rangedecay = 0.05f;
    vortices.append(v);
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

//    // Particles
    glColor3f(1,0,0);
    for (int i = 0; i < particles.size(); i++){
        particles[i].draw(sphereMesh);
    }

//    // Sphere
//    sphere.draw(sphereMesh);

//    // Vortices
//    if (drawVortices){
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_ONE, GL_ONE_MINUS_DST_COLOR);

//        glColor3f(0.5f,0.5f,0.9f);
//        for (int i = 0; i < vortices.size(); i++){
//              vortices[i].draw(sphereMesh);
//        }
//        glDisable(GL_BLEND);
//    }

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
    if (moveSphere){
        sphere.update(seconds);
    }
    else{
        sphere.stop();
    }

    for (int i = 0; i < particles.size(); i++){
        particles[i].update();
    }

    for (int i = 0; i < vortices.size(); i++){
        vortices[i].update(seconds);
    }

    dWorldQuickStep(m_world_id, 0.15f);
    dJointGroupEmpty(contactgroup);

    int toAdd;
    if (particles.size() > 1200)
        toAdd = 2;
    else
        toAdd = 6;

    for (int i = 0; i < toAdd; i++){
        addBody();
    }

    if (dRandReal() < 0.05)
        addVortex();

    for (int i = vortices.size() - 1; i >= 0; i--){
        if (!vortices[i].active){
            vortices.removeAt(i);
            continue;
        }
        const dReal *pos = dBodyGetPosition(vortices[i].body);
        if (pos[1] > MAX_HEIGHT){
            vortices[i].destroy();
            vortices.removeAt(i);
        }
    }

    // Remove from back of the list to avoid skipping elements
    for (int i = particles.size() - 1; i >= 0; i--){
        const dReal *pos = dBodyGetPosition(particles[i].body);
        if (pos[1] > MAX_HEIGHT){
            particles[i].destroy();
            particles.removeAt(i);
        }
    }
//    std::cout<<particles.size()<<std::endl;
}

