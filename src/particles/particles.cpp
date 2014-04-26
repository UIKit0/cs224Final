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

<<<<<<< HEAD

    // Load textures
    int texNum = 4;
    QImage textures[texNum];
    textures[0] = QImage("../res/smoke_alpha.png");
    textures[1] = QImage("../res/smoke_color.png");
    textures[2] = QImage("../res/smoke_depth.png");
    textures[3] = QImage("../res/smoke_normal.png");

    for(int i = 0; i < texNum; i++)
    {
        if (textures[i].isNull())
        {
            qCritical("Unable to load texture!");
            return;
        }
        textures[i].convertToFormat(QImage::Format_RGBA8888);
        m_gl->glGenTextures(1, &texHandles[i]);
        m_gl->glBindTexture(GL_TEXTURE_2D, texHandles[i]);

        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures[i].width(), textures[i].height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, textures[i].bits());

    }



=======
>>>>>>> 18aaf5d031d9bb7bb6703d13cfa2df08f67646ce
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

<<<<<<< HEAD
    m_gl->glActiveTexture(GL_TEXTURE0);
    m_gl->glBindTexture(GL_TEXTURE_2D,texHandles[0]);

    m_gl->glActiveTexture(GL_TEXTURE1);
    m_gl->glBindTexture(GL_TEXTURE_2D,texHandles[1]);

    m_gl->glActiveTexture(GL_TEXTURE2);
    m_gl->glBindTexture(GL_TEXTURE_2D,texHandles[2]);

    m_gl->glActiveTexture(GL_TEXTURE3);
    m_gl->glBindTexture(GL_TEXTURE_2D,texHandles[3]);


=======
//    qDebug() << data.size();
>>>>>>> 18aaf5d031d9bb7bb6703d13cfa2df08f67646ce
    glDrawArrays(GL_POINTS, 0, data.size());
}
