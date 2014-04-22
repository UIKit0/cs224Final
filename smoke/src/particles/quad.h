#ifndef QUAD_H
#define QUAD_H

#include <qgl.h>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class Quad
{
private:
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;

public:
    Quad();

    void draw();
};

#endif // QUAD_H
