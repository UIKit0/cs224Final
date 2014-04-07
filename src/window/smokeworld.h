#ifndef SMOKEWORLD_H
#define SMOKEWORLD_H

#include <QList>
#include <glm/vec3.hpp>

struct SmokeParticle{
    glm::vec3 location;
    glm::vec3 velocity;
};

class SmokeWorld
{
public:
    SmokeWorld();

    void render();

    void tick(float seconds);

private:
    QList<SmokeParticle> m_particles;
};

#endif // SMOKEWORLD_H
