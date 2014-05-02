#ifndef GLOBAL_H
#define GLOBAL_H

//TODO: move this graphics

#ifdef ENABLE_CORE_PROFILE
#include <QOpenGLFunctions_4_1_Core>
typedef QOpenGLFunctions_4_1_Core GLFunctions;
#else
#include <QOpenGLFunctions_2_0>
typedef QOpenGLFunctions_2_0 GLFunctions;
#endif

// use this to check if u have problems with opengl
// it should display a blue triangle
//#define DEBUG_TRIANGLE

#ifndef DEBUG_TRIANGLE
    #define TERRAIN
    #define CONTOUR
    #define PARTICLES
#endif

#include "matrixstack.h"
#include "camera.h"

// TODO: bad design the camera should be contained in world
extern Camera g_camera;

// TODO: same here the world should now about it's transformations
// if a class needs to know about world then pass a reference of world to it
extern MatrixStack g_model;

// Globals for the world id and mass for ode
#include "ode/ode.h"

extern dWorldID g_world;
extern dMass g_mass;

class Particles;
#include "QList"
extern QList<Particles*> g_particles;
extern QList<Particles*> g_free_particles;

#endif // GLOBAL_H
