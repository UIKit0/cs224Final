#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QtCore/QCoreApplication>
#include <QtGui/QWindow>

#include <QOpenGLFunctions_2_0>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLDebugLogger>

#ifndef ENABLE_CORE_PROFILE
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#endif

#include <QElapsedTimer>

#include <glm/ext.hpp>
#include <QDebug>

#include "graphics/debug.h"

class OpenGLWindow : public QWindow
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    virtual void initialize() =0;
    virtual void render() =0;
    virtual void render(QPainter *painter) =0;
    virtual void update(float seconds) =0;

    bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);

    void setGlobalGLState(const QVector<GLenum> &attribs);

    QOpenGLFunctions_2_0 *m_gl;
    QOpenGLContext *m_context;

private slots:
    void handleLogMessage(const QOpenGLDebugMessage &debugMessage);

private:
    bool m_update_pending;
    bool m_animating;

#ifndef ENABLE_CORE_PROFILE
    QOpenGLPaintDevice *m_device;
    QPainter *m_painter;
#endif

    QOpenGLDebugLogger *m_logger;

    QElapsedTimer m_tickTimer;
    QElapsedTimer m_fpsTimer;

    int m_fps;
    int m_frames;

    QSet<GLenum> m_glStates;
};

#endif // OPENGLWINDOW_H

