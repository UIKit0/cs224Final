#include "particles.h"

Particles::Particles()
    : m_gl(NULL)
{
}

Particles::Particles(GLFunctions *gl, int maxParticles)
{
    initialize(gl, maxParticles);
}

void Particles::initialize(GLFunctions *gl, int maxParticles)
{
    m_gl = gl;
    m_maxParticles = maxParticles;

    m_smokeFx.initialize(gl, "../res/shaders/");
    m_smokeFx.compile(GL_VERTEX_SHADER, "smoke.vertex.point");
    m_smokeFx.compile(GL_FRAGMENT_SHADER, "smoke.fragment.point");
    m_smokeFx.link();

    m_projUniform = m_smokeFx.uniform("proj_matrix");
    m_mvUniform = m_smokeFx.uniform("mv_matrix");

    m_posAttrib = m_smokeFx.attrib("position");
    m_sizeAttrib = m_smokeFx.attrib("size");

    m_gl->glGenVertexArrays(1, &m_vao);
    m_gl->glBindVertexArray(m_vao);

    m_gl->glGenBuffers(1, &m_buffer);

    data.reserve(maxParticles);

    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    m_gl->glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleBuffer) * maxParticles, data.data(), GL_DYNAMIC_DRAW);

}

void Particles::setBufferSize(int size)
{
    data.resize(size);
}

void Particles::setBufferValue(int index, glm::vec3 position, float size)
{
    Q_ASSERT_X(index < m_maxParticles && index >= 0, "bad buffer index", QString::number(index).toLocal8Bit());

    ParticleBuffer &particle = data[index];
    particle.position = position;
    particle.size = size;
}

void Particles::draw()
{
    Q_ASSERT(m_gl != NULL);

    m_gl->glBindVertexArray(m_vao);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    // update buffer
    m_gl->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleBuffer) * data.size(), data.data());

    m_gl->glUseProgram(m_smokeFx.program());
    m_gl->glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
    m_gl->glUniformMatrix4fv(m_mvUniform, 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix * g_model.mMatrix));

    quintptr offset = 0;

    m_gl->glEnableVertexAttribArray(m_posAttrib);
    m_gl->glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);

    offset += sizeof(glm::vec3);

    m_gl->glEnableVertexAttribArray(m_sizeAttrib);
    m_gl->glVertexAttribPointer(m_sizeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);

//    qDebug() << data.size();
    glDrawArrays(GL_POINTS, 0, data.size());
}
