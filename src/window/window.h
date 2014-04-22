#ifndef WINDOW_H
#define WINDOW_H

#include "openglwindow.h"
#include "scene/world.h"

#include <QtGui/QMouseEvent>

class Window : public OpenGLWindow
{
public:
    Window();
    ~Window();

protected:
    virtual void update(float seconds);
    void initialize();
    virtual void render();
    virtual void render(QPainter *brush);

private:
    // World
    World m_world;

    bool m_resetMouse;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // WINDOW_H
