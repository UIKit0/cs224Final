#include "window.h"

Window::Window()
    : m_resetMouse(true)
{

}

Window::~Window()
{

}

void Window::update(float seconds)
{
    OpenGLWindow::update(seconds);
    m_world.update(seconds);
}

void Window::initialize()
{
    m_gl->glClearColor(255/255.0f,
                       255/255.0f,
                       255/255.0f,
                       1);

    QVector<GLenum> attribs;
    attribs << GL_DEPTH_TEST
            << GL_CULL_FACE;
    setGlobalGLState(attribs);

    // world
    m_world.setScreenSize(width(), height());
    m_world.initialize(m_gl);
}

void Window::render()
{
    const qreal retinaScale = devicePixelRatio();
    m_gl->glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    m_gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_world.render(m_gl);
}

void Window::render(QPainter *brush)
{
    Q_UNUSED(brush);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    //  reset mouse position to center
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

    m_world.mouseMoveEvent(event);
    m_world.mouseMoveDelta(deltaX, deltaY);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QWindow::close();

    m_world.keyPressEvent(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    m_world.mousePressEvent(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    m_world.mouseReleaseEvent(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    m_world.keyReleaseEvent(event);
}
