#include "smoketrailemitter.h"

SmokeTrailEmitter::SmokeTrailEmitter(dWorldID w, dSpaceID s, dMass m) : ParticleEmitter(w, s, m)
{
    QImage img = QImage(":/textures/smoke2.png");

    if (img.isNull()) {
        qCritical("Unable to load texture!");
        return;
    }

    img = img.convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &sprites);
    glBindTexture(GL_TEXTURE_2D, sprites);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

}

void SmokeTrailEmitter::updateParticles()
{
    for (int i = 0; i < 1; i++){
        SmokeParticle sp = SmokeParticle(world, space, mass, dRandReal()*0.05f + 0.03f);

        dBodySetPosition(sp.body, location[0] + dRandReal() - 0.5f,
                         location[1] + dRandReal() - 0.5f,
                         location[2] + dRandReal() - 0.5f);

        sp.rotationSpeed = dRandReal() - 0.5f;

        // Actually make particles bigger with time, so there aren't random gaps
        sp.shrink = 0.1f;
        sp.minScale = 0.1f;
        sp.fade = -0.5f;
        sp.minAlpha = 0.02f;
        particles.append(sp);
    }
}

void SmokeTrailEmitter::updateVortices()
{
    // Do nothing
}
