#ifndef PARTICLE_H
#define PARTICLE_H

#include "scene/global.h"
#include "graphics/program.h"

#include "ode/ode.h"

/**
 * @brief The Particle class (rename to particle buffer)
 *
 * Represents a group of particles for drawing and physics.
 *
 * Implented using point sprites.
 *
 */
class Particles
{
public:
    Particles();
    Particles(GLFunctions *gl, int maxParticles = 1000);
    void initialize(GLFunctions *gl, int maxParticles = 1000);
    void setBufferValue(int index, glm::vec3 position, float size);
    void setBufferSize(int size);
    virtual void draw();

    struct ParticleBuffer {
        glm::vec3 position;
        GLfloat size;
    };

// Rendering only, no need to share with subclasses
private:
    GLFunctions *m_gl;

    Program m_depthPass;
    Program m_stencilPass;
    Program m_smokeFx;

    GLint m_sizeAttrib;
    GLint m_posAttrib;

    GLuint m_pUniform;
    GLuint m_vUniform;
    GLuint m_mvUniform;

    GLuint m_lightUniform;

    GLuint m_buffer;
    GLuint m_vao;

    // Depth shader attribute
    GLint m_depthPosAttrib;
    GLint m_depthSizeAttrib;
    GLint m_depthPUniform;
    GLint m_depthVUniform;

    // Stencil shader attributes
    GLint m_stencilPosAttrib;
    GLint m_stencilSizeAttrib;

    int m_maxParticles;
    int m_particles;

    GLuint m_texID[4];
    GLuint m_texUniform[4];

    QVector<ParticleBuffer> data;

    GLfloat m_lightpos[3];

    void renderDepthPass();
    void renderLightingPass();
    void renderStencilPass();
};

#endif // PARTICLE_H
