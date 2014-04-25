#ifndef GLOBAL_H
#define GLOBAL_H


#include <QOpenGLFunctions_2_0>
//#include <QOpenGLFunctions_4_1_Core>
//TODO: move this graphics
typedef QOpenGLFunctions_2_0 GLFunctions;
//typedef QOpenGLFunctions_4_1_Core GLFunctions;

#include "matrixstack.h"
#include "camera.h"

// TODO: bad design the camera should be contained in world
extern Camera g_camera;

// TODO: same here the world should now about it's transformations
// if a class needs to know about world then pass a reference of world to it
extern MatrixStack g_model;

#endif // GLOBAL_H
