#include "matrixstack.h"

MatrixStack::MatrixStack()
{
    mMatrix = glm::mat4(1.0f);
}

void MatrixStack::pushMatrix(){
    stack.append(mMatrix);
}

void MatrixStack::popMatrix(){
    Q_ASSERT(!stack.isEmpty());
    mMatrix = stack.last();
    stack.pop_back();
}

void MatrixStack::reset(){
    mMatrix = glm::mat4(1.0f);
    stack.clear();
}
