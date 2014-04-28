#include "world.h"

World::World()
    : m_screenWidth(500)
    , m_screenHeight(500)
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

World::~World()
{
    emitters.clear();
    dJointGroupDestroy(contactgroup);
    dSpaceDestroy(space);
    dWorldDestroy(m_world_id);
    dCloseODE();
}

void World::toggleDrawVortices()
{
    for (int i = 0; i < emitters.size(); i++)
    {
        emitters[i]->drawVortices = !emitters[i]->drawVortices;
    }
}

void World::toggleMovingSphere()
{
//    if (sphere.moving != 0)
//        sphere.stop();
//    else
//        sphere.start();
}

static void nearCallback(void* data, dGeomID o1, dGeomID o2)
{
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

void World::initialize(GLFunctions *gl)
{
    // camera
    g_camera.setAspectRatio((float)m_screenWidth/m_screenHeight);

#ifdef TERRAIN
    m_terrain.initialize(gl);
#endif

#ifdef PARTICLES
    BasicSmokeEmitter *emitter = new BasicSmokeEmitter(m_world_id, space, m);
    emitter->initialize(gl, 2000);
    emitter->maxInitialVel = glm::vec3(0.5f, 2.0f, 0.5f);
    emitter->minInitialVel = glm::vec3(-0.5f, 0.5f, -0.5f);
    emitters.append(emitter);
#endif

#ifdef CONTOUR
    Obj mesh("monkey.obj");
    m_contour.initialize(gl, mesh);
#endif

#ifdef DEBUG_TRIANGLE
    //    m_debugFx.initialize(gl, "../../../../res/shaders/");
    m_debugFx.initialize(gl, "../res/shaders/");
    m_debugFx.compile(GL_VERTEX_SHADER, "contour.vertex.debug");
    m_debugFx.compile(GL_FRAGMENT_SHADER, "contour.fragment.debug");
    m_debugFx.link();

    GLuint vertexArray;
    gl->glGenVertexArrays(1, &vertexArray);
    gl->glBindVertexArray(vertexArray);
#endif
}

void World::render(GLFunctions *gl)
{
    g_model.reset();

#ifdef TERRAIN
    m_terrain.draw();
#endif

#ifdef CONTOUR
    m_contour.draw();
#endif

#ifdef PARTICLES
    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->draw();
    }
#endif

#ifdef DEBUG_TRIANGLE
    gl->glUseProgram(m_debugFx.program());
    gl->glDrawArrays(GL_TRIANGLES, 0, 3);
#endif
}

void World::update(float seconds)
{
    g_camera.update(seconds);

#ifdef TERRAIN
    m_terrain.update(seconds, g_camera.m_position);
#endif

#ifdef PARTICLES
    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->update(seconds);
    }
#endif

    dWorldQuickStep(m_world_id, 1/30.0f);
//    dJointGroupEmpty(contactgroup);
}

void World::setScreenSize(int width, int height)
{
    m_screenHeight = height;
    m_screenWidth = width;
}

void World::mouseMoveDelta(float deltaX, float deltaY)
{
    g_camera.mouseRotation(glm::vec2(deltaX, deltaY));
}

void World::keyPressEvent(QKeyEvent *event)
{

    switch(event->key()) {
    case Qt::Key_W:
        g_camera.pressingForward = true;
        break;
    case Qt::Key_S:
        g_camera.pressingBackward = true;
        break;
    case Qt::Key_A:
        g_camera.pressingLeft = true;
        break;
    case Qt::Key_D:
        g_camera.pressingRight = true;
        break;
    }

    if (event->key() == Qt::Key_V) toggleDrawVortices();

    if (event->key() == Qt::Key_B) toggleMovingSphere();

    if (event->key() == Qt::Key_Space) g_camera.pressingJump = true;
}

void World::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_W:
        g_camera.pressingForward = false;
        break;
    case Qt::Key_S:
        g_camera.pressingBackward = false;
        break;
    case Qt::Key_A:
        g_camera.pressingLeft = false;
        break;
    case Qt::Key_D:
        g_camera.pressingRight = false;
        break;
    }

    if (event->key() == Qt::Key_Space) g_camera.pressingJump = false;
}

void World::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void World::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void World::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}
