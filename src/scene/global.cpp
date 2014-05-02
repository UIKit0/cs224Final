#include "scene/global.h"

MatrixStack g_model;
Camera g_camera;

dWorldID g_world;
dMass g_mass;

Particles* g_particles;
QList<ParticleEmitter*> g_emitters;
//QList<Particles*> g_free_particles;
