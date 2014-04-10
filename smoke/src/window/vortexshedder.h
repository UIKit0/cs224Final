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
 * Currently there is one main vortex that sticks to the object.
 *
 * Eventually, vortices will be generated as the object moves.
 * @brief The VortexShedder class
 */
class VortexShedder
{
public:
    VortexShedder(dWorldID w, dSpaceID s, dMass m, dBodyID o,
                  glm::vec3 loc, glm::mat3x3 at,
                  float vt, float vs,
                  float f, float r);

    void update(float seconds);
    void draw(Obj &obj);

    dBodyID owner;
    // Location relative to the owner's center
    glm::vec3 location;
    // Transformation from the velocity of the owner to the axis of the main vortex
    glm::mat3x3 axistransform;
    // Minimum velocity for vortex to be enabled
    float velThreshold;
    // Scaling factor of the various parameters based on velocity
    float velScale;
    // Base force
    float force;
    // Base range (large velocity makes for larger range)
    float range;

    dWorldID world;
    dSpaceID space;
    dMass mass;

    // Will eventually have more than one vortex in it
    QList<Vortex> vortices;
};

#endif // VORTEXSHEDDER_H
