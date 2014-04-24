#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <QList>

class MatrixStack
{
public:
    MatrixStack();

    void pushMatrix();
    void popMatrix();

    glm::mat4 matrix;
    QList<glm::mat4> stack;
};

#endif // MATRIXSTACK_H
