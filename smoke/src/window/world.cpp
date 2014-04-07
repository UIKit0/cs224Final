#include "world.h"

#define SPHERE_SIZE 2.0f
#define PARTICLE_SIZE 0.3f

#define VORTEX_CATEGORY_BITS 7
#define PARTICLE_CATEGORY_BITS 1

#define SPAWN_SIZE 3

#define MAX_HEIGHT 20

World::World()
{
    dInitODE();
    m_world_id = dWorldCreate();
    dWorldSetGravity(m_world_id, 0, 0, 0);
    dWorldSetLinearDamping(m_world_id, 0.01f);

    space = dSimpleSpaceCreate(0);

    for (int i = 0; i < 10; i++){
        addBody();
    }

    addVortex();

    object = dBodyCreate(m_world_id);
    dBodySetKinematic(object);
    dMassSetSphere(&m, 10.0f, SPHERE_SIZE);
    obj = dCreateSphere(space, SPHERE_SIZE);
    dGeomSetBody(obj, object);
    dBodySetPosition(object, 20, 10, 0);
    movingPosX = false;

    contactgroup = dJointGroupCreate(0);

    dAllocateODEDataForThread(dAllocateMaskAll);

    quad = gluNewQuadric();
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
            glm::vec3 f = (tangent + glm::cross(tangent, v.axis)*1.5f)*force;
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
    contact.surface.bounce = 0.1f;
    contact.surface.bounce_vel = dInfinity;
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
    dBodyID body = dBodyCreate(m_world_id);
    dGeomID geom = dCreateSphere(space, PARTICLE_SIZE);

    dGeomSetBody(geom, body);

    dGeomSetCategoryBits(geom, PARTICLE_CATEGORY_BITS);
    dGeomSetCollideBits(geom, ~PARTICLE_CATEGORY_BITS);

    // Maintain same mass
    dMassSetSphere(&m, 1/PARTICLE_SIZE, PARTICLE_SIZE);
    dBodySetMass(body, &m);

    dBodySetPosition(body, dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2,
                     dRandReal(),
                     dRandReal()*SPAWN_SIZE - SPAWN_SIZE/2);
    float maxInitialVel = 0.2f;
    float maxVerticalVel = 1.0f;
    dBodySetLinearVel(body, dRandReal()*maxInitialVel*2 - maxInitialVel,
                            dRandReal()*maxVerticalVel,
                            dRandReal()*maxInitialVel*2 - maxInitialVel);

    particles.append(Particle(body, geom));
}

void World::addVortex(){
    dBodyID vbody = dBodyCreate(m_world_id);
    dGeomID vgeom = dCreateSphere(space, 2.0f);
    dGeomSetBody(vgeom, vbody);

    dGeomSetCategoryBits(vgeom, VORTEX_CATEGORY_BITS);
    dGeomSetCollideBits(vgeom, ~VORTEX_CATEGORY_BITS);

    dBodySetKinematic(vbody);
    dBodySetPosition(vbody, dRandReal()*SPAWN_SIZE*2 - SPAWN_SIZE, 3, dRandReal()*SPAWN_SIZE*2 - SPAWN_SIZE);
    Vortex v = Vortex(vbody, vgeom);
    v.axis = glm::normalize(glm::vec3(dRandReal(), dRandReal(), dRandReal()));
    v.falloff = 2.0f;
    v.force = 0.2f;
    v.range = 2.0f;
    vortices.append(v);
}

void World::init()
{
    initializeOpenGLFunctions();
}

void World::draw()
{
    glPushMatrix();
    // Ground
    glColor3f(1,1,0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-100, 0, -100);
    glVertex3f(-100, 0, 100);
    glVertex3f(100, 0, 100);
    glVertex3f(-100, 0, -100);
    glVertex3f(100, 0, 100);
    glVertex3f(100, 0, -100);
    glEnd();

    // Particles
    glColor3f(1,0,0);
    for (int i = 0; i < particles.size(); i++){
        const dReal* pos = dBodyGetPosition(particles[i].body);

        glTranslatef(pos[0], pos[1], pos[2]);
        gluSphere(quad, PARTICLE_SIZE, 8, 8);
        glTranslatef(-pos[0], -pos[1], -pos[2]);
    }

    // Sphere
    glColor3f(0,1,0);
    const dReal* pos = dBodyGetPosition(object);

    glTranslatef(pos[0], pos[1], pos[2]);
    gluSphere(quad, SPHERE_SIZE, 16, 16);
    glTranslatef(-pos[0], -pos[1], -pos[2]);

    // Vortices
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_ONE, GL_ONE);

//    glColor3f(0,0,0.5f);
//    for (int i = 0; i < vortices.size(); i++){
//        const dReal* pos = dBodyGetPosition(vortices[i].body);

//        glTranslatef(pos[0], pos[1], pos[2]);
//        gluSphere(quad, vortices[i].range, 8, 8);
//        glTranslatef(-pos[0], -pos[1], -pos[2]);
//    }
//    glDisable(GL_BLEND);
    glPopMatrix();
}

void World::tick(float seconds){
    // Upwards force
    dSpaceCollide(space, this, nearCallback);
//    if (movingPosX){
//        dBodySetLinearVel(object, 2.0f, 0, 0);
//    }
//    else{
//        dBodySetLinearVel(object, -2.0f, 0, 0);
//    }
//    if (dBodyGetPosition(object)[0] > 20){
//        movingPosX = false;
//    }
//    if (dBodyGetPosition(object)[0] < -20){
//        movingPosX = true;
//    }

    for (int i = 0; i < particles.size(); i++){
        dBodyAddForce(particles[i].body, 0, dRandReal()*0.05f, 0);
        if (dRandReal() < 0.1){
            dBodyAddForce(particles[i].body, dRandReal()*0.2f - 0.1f, 0, dRandReal()*0.2f - 0.1f);
        }
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
        const dReal *pos = dBodyGetPosition(vortices[i].body);
        if (pos[1] > MAX_HEIGHT){
            dGeomDestroy(vortices[i].geom);
            dBodyDestroy(vortices[i].body);
            vortices.removeAt(i);
        }
        else{
            dBodySetPosition(vortices[i].body, pos[0], pos[1] + dRandReal()*0.1f, pos[2]);
        }
    }

    for (int i = particles.size() - 1; i >= 0; i--){
        const dReal *pos = dBodyGetPosition(particles[i].body);
        if (pos[1] > MAX_HEIGHT){
            dGeomDestroy(particles[i].geom);
            dBodyDestroy(particles[i].body);
            particles.removeAt(i);
        }
    }
//    std::cout<<particles.size()<<std::endl;
}

