#ifndef VORTEXSHEDDER_H
#define VORTEXSHEDDER_H

#include <ode/ode.h>

#include <glm/vec3.hpp>
#include <glm/ext.hpp>

//#include <OpenGL/glu.h>

#include <QList>

#include <vortex.h>
#include "assets/obj.h"
/**
 * Represents interaction between a moving object and smoke.
 * Generates vortices based on the velocity of the object.
 * The owning object should set the fields of the shedder before calling update().
 *
 * Eventually, vortices will be generated as the object moves.
 * @brief The VortexShedder class
 */
class VortexShedder
{
public:
    VortexShedder(dWorldID w, dSpaceID s, dMass m, dBodyID o);

    void update(float seconds);
    void draw(Obj &obj);
    void destroy();

    dBodyID owner;
    dWorldID world;
    dSpaceID space;
    dMass mass;

    // Axis of the vortices, to be varied slightly.
    glm::vec3 axis;
    // Scaling constant
    float velScale;
    // Threshold for when to generate vortices
    float velThreshold;

    // Base falloff
    float falloff;
    // Base force
    float force;
    // Base force decay
    float forcedecay;
    // Base centripetal force
    float centripetal;
    // Base range (large velocity makes for larger range)
    float range;
    // Base range decay
    float rangedecay;

    // Offset relative to the center of the owner
    glm::vec3 location;

    // List of vortices that this shedder has generated.
    QList<Vortex*> vortices;
};

#endif // VORTEXSHEDDER_H
