#include "matrixstack.h"

MatrixStack::MatrixStack()
{
    matrix = glm::mat4(1.0f);
}

void MatrixStack::pushMatrix(){
    stack.append(matrix);
}

void MatrixStack::popMatrix(){
    Q_ASSERT(!stack.isEmpty());
    matrix = stack.last();
    stack.pop_back();
}
