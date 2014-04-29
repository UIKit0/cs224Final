#include "particles.h"

#include <QImage>

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

//    m_smokeFx.initialize(gl, "../../../../res/shaders/");
    m_smokeFx.initialize(gl, "../res/shaders/");
    m_smokeFx.compile(GL_VERTEX_SHADER, "smoke.vertex.point");
    m_smokeFx.compile(GL_FRAGMENT_SHADER, "smoke.fragment.point");
    m_smokeFx.link();

    m_projUniform = m_smokeFx.uniform("proj_matrix");
    m_viewUniform = m_smokeFx.uniform("view_matrix");
    m_mvUniform = m_smokeFx.uniform("mv_matrix");
    m_lightUniform = m_smokeFx.uniform("lightPos");

    m_posAttrib = m_smokeFx.attrib("position");
    m_sizeAttrib = m_smokeFx.attrib("size");

    m_gl->glGenVertexArrays(1, &m_vao);
    m_gl->glBindVertexArray(m_vao);

    m_gl->glGenBuffers(1, &m_buffer);

    data.reserve(maxParticles);

    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    m_gl->glBufferData(GL_ARRAY_BUFFER,
                       sizeof(ParticleBuffer) * maxParticles, data.data(),
                       GL_DYNAMIC_DRAW);

    // Load textures
    int texNum = 4;
    QImage textures[texNum];
    textures[0] = QImage(":/textures/smoke_alpha.png");
    textures[1] = QImage(":/textures/smoke_color.png");
    textures[2] = QImage(":/textures/smoke_depth.png");
    textures[3] = QImage(":/textures/smoke_normal.png");

    m_texUniform[0] = m_smokeFx.uniform("tex_alpha");
    m_texUniform[1] = m_smokeFx.uniform("tex_color");
    m_texUniform[2] = m_smokeFx.uniform("tex_depth");
    m_texUniform[3] = m_smokeFx.uniform("tex_norm");

    GLenum Tslots[4];
    Tslots[0] = GL_TEXTURE0;
    Tslots[1] = GL_TEXTURE1;
    Tslots[2] = GL_TEXTURE2;
    Tslots[3] = GL_TEXTURE3;

    m_gl->glGenTextures(texNum, m_texID);

    for(int i = 0; i < texNum; i++)
    {
        if (textures[i].isNull())
        {
            qCritical("Unable to load texture!");
            return;
        }

        QImage img = textures[i].convertToFormat(QImage::Format_RGBA8888);

        m_gl->glActiveTexture(Tslots[i]);
        m_gl->glBindTexture(GL_TEXTURE_2D, m_texID[i]);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    }
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

    m_gl->glEnable(GL_PROGRAM_POINT_SIZE);
    m_gl->glUseProgram(m_smokeFx.program());

    // Update buffer
    m_gl->glBindVertexArray(m_vao);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    m_gl->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleBuffer) * data.size(), data.data());

    m_gl->glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
    m_gl->glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix));
    m_gl->glUniformMatrix4fv(m_mvUniform, 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix * g_model.mMatrix));
    m_gl->glUniform3f(m_lightUniform, 0.0f, 10.0f, 4.0f);

    // Vertex attributes
    quintptr offset = 0;
    m_gl->glEnableVertexAttribArray(m_posAttrib);
    m_gl->glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);
    offset += sizeof(glm::vec3);
    m_gl->glEnableVertexAttribArray(m_sizeAttrib);
    m_gl->glVertexAttribPointer(m_sizeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);

    // Bind textures
    for(GLuint i = 0; i < 4; i++)
    {
        m_gl->glUniform1i(m_texUniform[i], i);
        m_gl->glActiveTexture(GL_TEXTURE0+i);
        m_gl->glBindTexture(GL_TEXTURE_2D, m_texID[i]);
    }

    glDrawArrays(GL_POINTS, 0, data.size());
}
