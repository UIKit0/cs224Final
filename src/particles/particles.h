#ifndef PARTICLE_H
#define PARTICLE_H

#include "scene/global.h"
#include "graphics/program.h"

/**
 * @brief The Particle class (rename to particle buffer)
 *
 * Represents a group of particles for drawing.
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

private:
    GLFunctions *m_gl;

    Program m_smokeFx;

    GLint m_sizeAttrib;
    GLint m_posAttrib;

    GLuint m_projUniform;
    GLuint m_viewUniform;
    GLuint m_mvUniform;

    GLuint m_lightUniform;

    GLuint m_buffer;
    GLuint m_vao;

    int m_maxParticles;

    GLuint m_texID[4];
    GLuint m_texUniform[4];

    QVector<ParticleBuffer> data;
};

#endif // PARTICLE_H
