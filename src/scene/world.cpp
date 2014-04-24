#include "world.h"

//#define DEBUG_TEST_TRIANGLE
#define PARTICLES

World::World()
    : m_screenWidth(500)
    , m_screenHeight(500)
    , m_mesh("monkey2.obj")
    , sphereMesh("cube.obj")
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

void World::initialize(QOpenGLFunctions_4_1_Core *gl)
{
    // camera
    m_camera.setAspectRatio((float)m_screenWidth/m_screenHeight);

    m_goochFx.initialize(gl, "../../../../res/shaders/");
//    m_goochFx.initialize(gl, "../res/shaders/");

#ifdef DEBUG_TEST_TRIANGLE
    m_goochFx.compile(GL_VERTEX_SHADER, "contour.vertex.debug");
    m_goochFx.compile(GL_FRAGMENT_SHADER, "contour.fragment.debug");
    m_goochFx.link();

    GLuint vertexArray;
    gl->glGenVertexArrays(1, &vertexArray);
    gl->glBindVertexArray(vertexArray);
#else
#ifndef PARTICLES
    m_goochFx.compile(GL_VERTEX_SHADER, "contour.vertex");
    m_goochFx.compile(GL_FRAGMENT_SHADER, "contour.fragment");
//    m_goochFx.compile(GL_GEOMETRY_SHADER, "contour.geometry");
    m_goochFx.link();

    QHash<QPair<int,int>, Adjacent> edgeMap;
    for(Obj::Triangle tri : m_mesh.triangles) {
        for (int i = 0; i < 3; ++i) {
            int ia = tri.indices[i].vertex;
            int ib = tri.indices[(i + 1) % 3].vertex;
            QPair<int, int> p(qMin(ia, ib), qMax(ia, ib));
            if (!edgeMap.contains(p)) {
//                edgeMap.insert(p, )
            } else {

            }
        }
    }

    // TODO: move to a obj / buffer class
    GLuint meshSize = m_mesh.triangles.size() * 3;
    QVector<MeshBuffer> data;
    data.reserve(meshSize);

    for(Obj::Triangle tri : m_mesh.triangles) {
        MeshBuffer a;
        a.position = m_mesh.vertices[tri.a.vertex];
        a.normal = m_mesh.normals[tri.a.normal];
        MeshBuffer b;
        b.position = m_mesh.vertices[tri.b.vertex];
        b.normal = m_mesh.normals[tri.b.normal];
        MeshBuffer c;
        c.position = m_mesh.vertices[tri.c.vertex];
        c.normal = m_mesh.normals[tri.c.normal];

        data.append(a);
        data.append(b);
        data.append(c);
    }

    gl->glGenVertexArrays(1, &m_vao);
    gl->glBindVertexArray(m_vao);

    gl->glGenBuffers(1, &m_buffer);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(World::MeshBuffer) * data.size(), data.data(), GL_STATIC_DRAW);

    quintptr offset = 0;

    gl->glEnableVertexAttribArray(m_goochFx.attrib("position"));
    gl->glVertexAttribPointer(m_goochFx.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);

    offset += sizeof(glm::vec3);

    gl->glEnableVertexAttribArray(m_goochFx.attrib("normal"));
    gl->glVertexAttribPointer(m_goochFx.attrib("normal"), 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);

    offset += sizeof(glm::vec3);

//    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (void *)offsetof(MeshBuffer, texcoord));
//    gl->glEnableVertexAttribArray(m_goochFx.attrib("texcoord"));
#endif

    BasicSmokeEmitter *emitter = new BasicSmokeEmitter(m_world_id, space, m);
    emitter->maxInitialVel = glm::vec3(0.5f, 2.0f, 0.5f);
    emitter->minInitialVel = glm::vec3(-0.5f, 0.5f, -0.5f);
    emitters.append(emitter);

    circlingEmitter = new SmokeTrailEmitter(m_world_id, space, m);
    circlingEmitter->location = glm::vec3(30,0,0);

    sphere = SolidObject(m_world_id, space, m);

#endif
}

void World::render(QOpenGLFunctions_4_1_Core *gl)
{
#ifdef DEBUG_TEST_TRIANGLE
    gl->glUseProgram(m_goochFx.program());
    gl->glDrawArrays(GL_TRIANGLES, 0, 3);
#else
#ifndef PARTICLES
    gl->glUseProgram(m_goochFx.program());

    // normal matrix
    glm::inverseTranspose(m_camera.vMatrix);

    gl->glUniformMatrix4fv(m_goochFx.uniform("proj"), 1, GL_FALSE, glm::value_ptr(m_camera.pMatrix));
    gl->glUniformMatrix4fv(m_goochFx.uniform("mv"), 1, GL_FALSE, glm::value_ptr(m_camera.vMatrix));

    gl->glUniform3f(m_goochFx.uniform("lightPos"), 0.0f, 10.0f, 4.0f);
    gl->glUniform3f(m_goochFx.uniform("surfaceColor"), 0.4f, 0.75f, 0.75f);
    gl->glUniform3f(m_goochFx.uniform("warmColor"), 0.6f, 0.6f, 0.0f);
    gl->glUniform3f(m_goochFx.uniform("coolColor"), 0.0f, 0.1f, 0.6f);
    gl->glUniform1f(m_goochFx.uniform("diffuseWarm"), 0.45f);
    gl->glUniform1f(m_goochFx.uniform("diffuseCool"), 0.15f);

    gl->glDrawArrays(GL_TRIANGLES, 0, m_mesh.triangles.size() * 3);
#endif


    // RENDER PARTICLES

    // Sphere
    sphere.draw(sphereMesh);

    circlingEmitter->draw(sphereMesh);

    for (int i = 0; i < emitters.size(); i++){
        emitters[i]->draw(sphereMesh);
    }

#endif
}

void World::update(float seconds)
{
    m_camera.update(seconds);

    // PARTICLES

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

void World::setScreenSize(int width, int height)
{
    m_screenHeight = height;
    m_screenWidth = width;
}

void World::mouseMoveDelta(float deltaX, float deltaY)
{
    m_camera.mouseRotation(glm::vec2(deltaX, deltaY));
}

void World::keyPressEvent(QKeyEvent *event)
{

    switch(event->key()) {
    case Qt::Key_W:
        m_camera.pressingForward = true;
        break;
    case Qt::Key_S:
        m_camera.pressingBackward = true;
        break;
    case Qt::Key_A:
        m_camera.pressingLeft = true;
        break;
    case Qt::Key_D:
        m_camera.pressingRight = true;
        break;
    }

    if (event->key() == Qt::Key_Space) m_camera.pressingJump = true;
}

void World::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_W:
        m_camera.pressingForward = false;
        break;
    case Qt::Key_S:
        m_camera.pressingBackward = false;
        break;
    case Qt::Key_A:
        m_camera.pressingLeft = false;
        break;
    case Qt::Key_D:
        m_camera.pressingRight = false;
        break;
    }

    if (event->key() == Qt::Key_Space) m_camera.pressingJump = false;
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
