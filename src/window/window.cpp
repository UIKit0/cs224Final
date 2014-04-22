#include "window.h"

Window::Window()
    : m_resetMouse(true)
    , m_program(0)
    , m_mesh("monkey.obj")
{

}

Window::~Window()
{

}


//	"v_texcoord = a_texcoord;\n"
//	"gl_Position = matrix * vec4(a_position, 1);\n"

GLuint Window::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void Window::onTick(const float seconds)
{
    OpenGLWindow::onTick(seconds);
    m_camera.update(seconds);
}

void Window::initialize()
{
    glClearColor(1,1,1,1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // camera
    m_camera.setAspectRatio(width()/height());

    // world
//    m_world.init();

    // shaders

    QString coolToWarmVertFile("../shaders/cooltowarm.vert");
    QString coolToWarmFragFile("../shaders/cooltowarm.frag");

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, coolToWarmVertFile);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, coolToWarmFragFile);

    m_program->link();

    m_posAttr = m_program->attributeLocation("a_position");
    m_texAttr = m_program->attributeLocation("a_texcoord");

//    m_matrixUniform = m_program->uniformLocation("matrix");
//    m_textureUniform = m_program->uniformLocation("texture");

    m_program->uniformLocation("LightPosition");
    m_program->uniformLocation("SurfaceColor");
    m_program->uniformLocation("WarmColor");
    m_program->uniformLocation("CoolColor");
    m_program->uniformLocation("DiffuseWarm");
    m_program->uniformLocation("DiffuseCool");

    // textures

    QImage img(":textures/cube2.png");

    if (img.isNull()) {
        qCritical("Unable to load texture!");
        return;
    }

    img = img.convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    getErrors("initialize");
}

void Window::getErrors(QString location) {
#ifndef QT_NO_DEBUG_OUTPUT

    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
        qDebug() << "OpenGL Error: " + location;

    switch(error) {
    case GL_INVALID_ENUM:
        qDebug() << "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        qDebug() << "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        qDebug() << "GL_INVALID_OPERATION";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        qDebug() << "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        qDebug() << "GL_OUT_OF_MEMORY";
        break;
    }

#endif
}

void Window::renderOpenGL()
{

    getErrors("rendering START");

    glEnable(GL_DEPTH_TEST);

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_camera.pMatrix));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(m_camera.vMatrix));

    m_program->bind();

//    glm::mat4 mvp = m_camera.pMatrix * m_camera.vMatrix;
//    glUniformMatrix4fv(m_matrixUniform, 1, GL_FALSE, glm::value_ptr(mvp));
//    m_program->setUniformValue("texture", 0);

    m_program->setUniformValue("LightPosition", 0.0f, 10.0f, 4.0f);
    m_program->setUniformValue("SurfaceColor", 0.4f, 0.75f, 0.75f);
    m_program->setUniformValue("WarmColor", 0.6f, 0.6f, 0.0f);
    m_program->setUniformValue("CoolColor", 0.0f, 0.1f, 0.6f);
    m_program->setUniformValue("DiffuseWarm", 0.45f);
    m_program->setUniformValue("DiffuseCool", 0.15f);

//    m_world.drawWorld(m_program);
    m_mesh.draw();
    m_program->release();

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

//    glDisableVertexAttribArray(m_posAttr);
//    glDisableVertexAttribArray(m_texAttr);


    glDisable(GL_DEPTH_TEST);
    getErrors("rendering END");
}

void Window::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement.

    // Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    QPoint viewCenter(width() / 2, height() / 2);

    if (m_resetMouse) {
        QCursor::setPos(mapToGlobal(viewCenter));
        if (event->x() == viewCenter.x() && event->y() == viewCenter.y())
            m_resetMouse = false;
        return;
    }

    int deltaX = event->x() - viewCenter.x();
    int deltaY = event->y() - viewCenter.y();

    if (!deltaX && !deltaY) return;
    QCursor::setPos(mapToGlobal(viewCenter));

    m_camera.mouseRotation(glm::vec2(deltaX, deltaY));
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QWindow::close();

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

void Window::keyReleaseEvent(QKeyEvent *event)
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
