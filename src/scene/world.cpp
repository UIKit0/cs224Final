#include "world.h"

World::World()
    : m_screenWidth(500)
    , m_screenHeight(500)
    , firing(false)
{

}

World::~World()
{
    dJointGroupDestroy(contactgroup);
    dSpaceDestroy(space);
    dWorldDestroy(m_world_id);
    dCloseODE();
}

void World::initialize(GLFunctions *gl)
{
    m_gl = gl;
    // camera
    g_camera.setAspectRatio((float)m_screenWidth/m_screenHeight);

    g_particles = new Particles(gl, 2000);

#ifdef TERRAIN
    m_terrain.initialize(gl);
#endif

#ifdef PLAYER
    player = new Player(space, &m_terrain);
    player->initialize(gl);
#endif

    enemies.append(new Enemy(gl, space, glm::vec3(0,5.0f,-5.0f), glm::vec3(dRandReal()*(float)M_PI, 0, 0)));

#ifdef PARTICLES
    BasicSmokeEmitter *emitter = new BasicSmokeEmitter(g_particles);
    emitter->location = glm::vec3(3,0,0);
    emitter->maxInitialVel = glm::vec3(0.3f, 1.0f, 0.3f);
    emitter->minInitialVel = glm::vec3(-0.3f, 0.5f, -0.3f);
#endif

#ifdef CONTOUR
    // TODO: support multiple contour drawn meshes
    QString f("feisar.obj");
//    Obj f("cube.obj");
    Obj mesh(f);
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

#ifdef PLAYER
    if (player->active){
        player->draw();
    }
#endif

#ifdef TERRAIN
    m_terrain.draw();
#endif

    for (int i = 0; i < enemies.size(); i++){
        enemies[i]->draw();
    }

#ifdef CONTOUR
    g_model.pushMatrix();
    g_model.mMatrix = glm::translate(glm::mat4(), glm::vec3(5,0,5));
    m_contour.draw();
    g_model.popMatrix();
#endif

#ifdef PARTICLES
    for (int i = 0; i < g_emitters.size(); i++){
        g_emitters[i]->draw();
    }
#endif

#ifdef DEBUG_TRIANGLE
    gl->glUseProgram(m_debugFx.program());
    gl->glDrawArrays(GL_TRIANGLES, 0, 3);
#endif
}

void World::update(float seconds)
{
    if (player->active){
        g_camera.update(seconds);

#ifdef PLAYER
        player->facing = g_camera.m_lookAt;
        player->up = glm::vec3(0,1.0f,0);
        player->left = glm::normalize(glm::cross(player->up, player->facing));
        player->rotation = g_camera.m_lastRotation;
        player->roll = glm::mix(player->roll, g_camera.m_rotation[0] - g_camera.m_lastRotation[0], 0.1f);
        player->pitch = glm::mix(player->pitch, g_camera.m_rotation[1] - g_camera.m_lastRotation[1], 0.1f);
        player->location = g_camera.m_position - 1.0f*glm::cross(player->facing, player->left);

        player->update(seconds);

        if (firing)
            player->fire();
    }
#endif

#ifdef TERRAIN
    m_terrain.update(seconds, g_camera.m_position);
#endif

    for (int i = enemies.size() - 1; i >= 0; i--){
        enemies[i]->target = player->location;
        enemies[i]->update(seconds);
        if (!enemies[i]->active){
            enemies[i]->destroy();
            enemies.removeAt(i);
        }
    }

#ifdef PARTICLES
    for (int i = g_emitters.size() - 1; i >= 0; i--){
        g_emitters[i]->update(seconds);
        if (dSpaceQuery(space, (dGeomID)g_emitters[i]->space) == 0)
            dSpaceAdd(space, (dGeomID)g_emitters[i]->space);

        if (!g_emitters[i]->isActive()){
            g_emitters[i]->destroy();
            g_emitters.removeAt(i);
        }
    }
#endif
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

    if (event->key() == Qt::Key_R) {
        for (int i = enemies.size() - 1; i >= 0; i--){
            enemies[i]->destroy();
            enemies.removeAt(i);
        }

        for (int i = g_emitters.size() - 1; i >= 0; i--){
            g_emitters[i]->destroy();
            g_emitters.removeAt(i);
        }
        g_camera = Camera();
        initialize(m_gl);
    }

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
    firing = true;
    if (player->active)
        player->fire();
}

void World::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void World::mouseReleaseEvent(QMouseEvent *event)
{
    firing = false;
}
