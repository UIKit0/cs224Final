#include "quad.h"

#include <qgl.h>
#include <QOpenGLShaderProgram>

#include "world/world.h"

Quad::Quad()
{
    QOpenGLShaderProgram* s = World::m_shader;

    GLfloat rad = 1.0f;
    GLfloat data[16+8] = { -rad, -rad, 0,   0, 0,
                            rad, -rad, 0,   1, 0,
                           -rad,  rad, 0,   0, 1,
                            rad,  rad, 0,   1, 1};

    size_t stride = sizeof(GLfloat)*3 + sizeof(GLfloat)*2;

//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
    vao.create();
    vao.bind();

//    g.glGenBuffers(1, &vbo);
//    g.glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    g.glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(12+8), &data[0], GL_STATIC_DRAW);
    vbo.create();
    vbo.bind();
    vbo.allocate(data, sizeof(GLfloat)*(12+8));

//    glEnableVertexAttribArray(0);   // Position
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    s->enableAttributeArray(0);
    s->setAttributeBuffer(0,GL_FLOAT,0,3,stride);

//    glEnableVertexAttribArray(1);   // Tex
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*) (3*sizeof(GLfloat)));
    s->enableAttributeArray(1);
    s->setAttributeBuffer(1,GL_FLOAT,3*sizeof(GLfloat),3,stride);

//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vao.release();
    vbo.release();

}

void Quad::draw()
{
    vao.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    vao.release();

//    glBindVertexArray(vao);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glBindVertexArray(0);
}
