#ifndef GLOBAL_H
#define GLOBAL_H

#include <QOpenGLFunctions_2_0>
//#include <QOpenGLFunctions_4_1_Core>
#include "matrixstack.h"
#include "camera.h"

//TODO: move this somewhere else
typedef QOpenGLFunctions_2_0 GLFunctions;
//typedef QOpenGLFunctions_4_1_Core GLFunctions;

extern MatrixStack g_model;
extern Camera g_camera;

#endif // GLOBAL_H
