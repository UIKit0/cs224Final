#ifndef SMOKEEMITTER_H
#define SMOKEEMITTER_H

#include <QtGui/QOpenGLFunctions>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

#include <QList>
#include <QImage>
#include <QGLContext>

#include <PxPhysicsAPI.h>

#include "particles/smokeparticle.h"
#include "assets/obj.h"

using namespace physx;

class ParticleEmitter
{
public:
    ParticleEmitter(PxPhysics *phys, PxScene *scene, PxMaterial *mat);

    PxPhysics*				phys;
    PxScene*				scene;
    PxMaterial*				material;

    void destroy();
    void update(float seconds);
    void draw(Obj &obj);

    virtual void updateParticles() = 0;
    virtual void updateVortices() = 0;

    QList<SmokeParticle> particles;

//    QList<Vortex*> vortices;

    bool drawVortices;

    float time;
    unsigned int sprites;

    glm::vec3 location;
};

#endif // SMOKEEMITTER_H
