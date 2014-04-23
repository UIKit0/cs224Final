#include "openglwindow.h"


OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent)
    , m_update_pending(false)
    , m_animating(false)
    , m_context(0)
    , m_fps(0)
    , m_frames(0)
{
    setSurfaceType(QWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setSamples(16); // multi-sampling
    format.setRenderableType(QSurfaceFormat::OpenGL); // change to opengles on mobile
#ifdef ENABLE_CORE_PROFILE
    format.setProfile(QSurfaceFormat::CoreProfile);
#else
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
#endif
#ifdef DEBUG_OPENGL
    format.setOption(QSurfaceFormat::DebugContext);
#endif
    format.setMajorVersion(4);
    format.setMinorVersion(1);
    format.setDepthBufferSize(24);
    setFormat(format);

    // Hide the cursor since this is a fullscreen app
    setCursor(Qt::BlankCursor);

    m_glStates << GL_MULTISAMPLE;

#ifndef ENABLE_CORE_PROFILE
    m_device = 0;
#endif
}

OpenGLWindow::~OpenGLWindow()
{
#ifndef ENABLE_CORE_PROFILE
    delete m_device;
#endif
}

void OpenGLWindow::update(float seconds)
{
    Q_UNUSED(seconds);

    ++m_frames;

    if (m_fpsTimer.elapsed() > 1000) {
        m_fps = glm::round((m_frames * 1000.0f) / m_fpsTimer.elapsed());
        m_fpsTimer.restart();
        m_frames = 0;
        qDebug() << "FPS:" << m_fps;
    }
}

void OpenGLWindow::renderLater()
{
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        m_update_pending = false;
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void OpenGLWindow::renderNow()
{
    if (!isExposed())
        return;

    // initialize opengl context

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());

        if (m_context->create()) {
            m_gl = m_context->versionFunctions<QOpenGLFunctions_4_1_Core>();
            if (!m_gl) {
                qCritical("Critical: Unable to initialize OpenGL 4.2 Functions");
                exit(EXIT_FAILURE);
            }

#ifdef DEBUG_OPENGL
            m_logger = new QOpenGLDebugLogger(this);
#endif
            needsInitialize = true;

            m_tickTimer.start();
            m_fpsTimer.start();
        } else {
            qWarning("Warning: Unable to create OpenGL context");
            return;
        }
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        m_gl->initializeOpenGLFunctions();
#ifdef DEBUG_OPENGL
        if (m_context->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
            m_logger->initialize();
            connect(m_logger,
                    &QOpenGLDebugLogger::messageLogged,
                    this,
                    &OpenGLWindow::handleLogMessage,
                    Qt::DirectConnection);
            m_logger->enableMessages();
            m_logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
            const QList<QOpenGLDebugMessage> &preInitErrors = m_logger->loggedMessages();
            if (preInitErrors.size()) {
                qWarning() << "Warning: Pre-Initialization OpenGL Errors (Qt's fault)";
                qWarning() << "--[" << preInitErrors << "]--";
            }
        } else {
            qWarning("OpenGL KHR Debugging not available.");
        }
#endif
        initialize();
    }

    // check initialize qpainter context
#ifndef ENABLE_CORE_PROFILE
    if (!m_device) {
        m_device = new QOpenGLPaintDevice;
        m_device->setSize(size());

        m_painter = new QPainter(m_device);
        m_painter->setRenderHint(QPainter::Antialiasing);
        m_painter->setRenderHint(QPainter::TextAntialiasing);
    }

    if (!m_painter->isActive()) {
        m_painter->begin(m_device);
    }
#endif

    float seconds = m_tickTimer.restart() * 0.001f;
    if (seconds > 1.0f) seconds = 1.0f;

    update(seconds);

#ifdef ENABLE_CORE_PROFILE
    for (GLenum attrib : m_glStates)
        m_gl->glEnable(attrib);
    // render opengl
    render();
    for (GLenum attrib : m_glStates)
        m_gl->glDisable(attrib);
#else
    m_painter->beginNativePainting();
    for (GLenum attrib : m_glAttributes)
        m_gl->glEnable(attrib);

    // render opengl
    render();

    for (GLenum attrib : m_glAttributes)
        m_gl->glDisable(attrib);
    m_painter->endNativePainting();

    // render with painter
    render(m_painter);

    m_painter->setPen(Qt::red);
    m_painter->setFont(QFont("Monospace", 11));
    m_painter->drawText(QRect(0,0,100,100), Qt::AlignCenter, "FPS: " + QString::number(m_fps));

    m_painter->end();
#endif

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}

void OpenGLWindow::handleLogMessage(const QOpenGLDebugMessage &debugMessage)
{
    qDebug() << debugMessage;

//    if (debugMessage.severity() == QOpenGLDebugMessage::HighSeverity)
//        exit(EXIT_FAILURE);
}

void OpenGLWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}

void OpenGLWindow::setGlobalGLState(const QVector<GLenum> &attribsList)
{
    for (GLenum attrib : attribsList)
        m_glStates << attrib;
}


