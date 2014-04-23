#include "world.h"

//#define DEBUG_TEST_TRIANGLE

World::World()
    : m_screenWidth(500)
    , m_screenHeight(500)
    , m_mesh("monkey2.obj")
{

}


void World::initialize(QOpenGLFunctions_4_2_Core *gl)
{
    // camera
    m_camera.setAspectRatio((float)m_screenWidth/m_screenHeight);

    m_goochFx.initialize(gl, "../res/shaders/");

#ifdef DEBUG_TEST_TRIANGLE
    m_goochFx.compile(GL_VERTEX_SHADER, "contour.vertex.debug");
    m_goochFx.compile(GL_FRAGMENT_SHADER, "contour.fragment.debug");
    m_goochFx.link();

    GLuint vertexArray;
    gl->glGenVertexArrays(1, &vertexArray);
    gl->glBindVertexArray(vertexArray);
#else

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
        data.append(MeshBuffer( m_mesh.vertices[tri.a.vertex], m_mesh.normals[tri.a.normal], glm::vec2()));
        data.append(MeshBuffer( m_mesh.vertices[tri.b.vertex], m_mesh.normals[tri.b.normal], glm::vec2()));
        data.append(MeshBuffer( m_mesh.vertices[tri.c.vertex], m_mesh.normals[tri.c.normal], glm::vec2()));
    }

    gl->glGenVertexArrays(1, &m_vao);
    gl->glBindVertexArray(m_vao);

    gl->glGenBuffers(1, &m_buffer);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(MeshBuffer) * data.size(), data.data(), GL_STATIC_DRAW);

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
}

void World::render(QOpenGLFunctions_4_2_Core *gl)
{
#ifdef DEBUG_TEST_TRIANGLE
    gl->glUseProgram(m_goochFx.program());
    gl->glDrawArrays(GL_TRIANGLES, 0, 3);
#else

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
}

void World::update(float seconds)
{
    m_camera.update(seconds);
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
