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

//    m_smokeFx.initialize(gl, "../../res/shaders/");
    m_smokeFx.initialize(gl, "../res/shaders/");
    m_smokeFx.compile(GL_VERTEX_SHADER, "smoke.vertex.point");
    m_smokeFx.compile(GL_FRAGMENT_SHADER, "smoke.fragment.point");
    m_smokeFx.link();
    m_smokeFx.use();

    m_projUniform = m_smokeFx.uniform("proj_matrix");
    m_mvUniform = m_smokeFx.uniform("mv_matrix");

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
    textures[0] = textures[0].convertToFormat(QImage::Format_RGBA8888);
    textures[1] = QImage(":/textures/smoke_color.png");
    textures[1] = textures[1].convertToFormat(QImage::Format_RGBA8888);
    textures[2] = QImage(":/textures/smoke_depth.png");
    textures[2] = textures[2].convertToFormat(QImage::Format_RGBA8888);
    textures[3] = QImage(":/textures/smoke_normal.png");
    textures[3] = textures[3].convertToFormat(QImage::Format_RGBA8888);

    m_gl->glGenTextures(texNum, texHandles);

    GLint alphaLoc  = m_gl->glGetUniformLocation(m_smokeFx.program(), "tex_alpha");
    GLint colorLoc  = m_gl->glGetUniformLocation(m_smokeFx.program(), "tex_color");
    GLint depthLoc  = m_gl->glGetUniformLocation(m_smokeFx.program(), "tex_depth");
    GLint normalLoc = m_gl->glGetUniformLocation(m_smokeFx.program(), "tex_norm");

    m_gl->glUniform1i(alphaLoc,0);
    m_gl->glUniform1i(colorLoc,1);
    m_gl->glUniform1i(depthLoc,2);
    m_gl->glUniform1i(normalLoc,3);

    printf("alphaLoc:  %d\n",alphaLoc);
    printf("colorLoc:  %d\n",colorLoc);
    printf("depthLoc:  %d\n",depthLoc);
    printf("normalLoc:  %d\n",normalLoc);

    GLenum Tslots[4];
    Tslots[0] = GL_TEXTURE0;
    Tslots[1] = GL_TEXTURE1;
    Tslots[2] = GL_TEXTURE2;
    Tslots[3] = GL_TEXTURE3;

    for(int i = 0; i < texNum; i++)
    {
        if (textures[i].isNull())
        {
            qCritical("Unable to load texture!");
            return;
        }

        m_gl->glActiveTexture(Tslots[i]);
        m_gl->glBindTexture(GL_TEXTURE_2D, texHandles[i]);
//        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        m_gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures[i].width(), textures[i].height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, textures[i].bits());
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
    m_smokeFx.use();

    // Update buffer
    m_gl->glBindVertexArray(m_vao);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    m_gl->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleBuffer) * data.size(), data.data());

    // Update camera info
    m_gl->glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
    m_gl->glUniformMatrix4fv(m_mvUniform, 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix * g_model.mMatrix));

    // Enable proper vertex attributes
    quintptr offset = 0;
    m_gl->glEnableVertexAttribArray(m_posAttrib);
    m_gl->glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);
    offset += sizeof(glm::vec3);
    m_gl->glEnableVertexAttribArray(m_sizeAttrib);
    m_gl->glVertexAttribPointer(m_sizeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);

    // Bind textures
    GLenum Tslots[4];
    Tslots[0] = GL_TEXTURE0;
    Tslots[1] = GL_TEXTURE1;
    Tslots[2] = GL_TEXTURE2;
    Tslots[3] = GL_TEXTURE3;
    for(int i = 0; i < 4; i++)
    {
        m_gl->glActiveTexture(Tslots[i]);
        m_gl->glBindTexture(GL_TEXTURE_2D, texHandles[i]);
    }



    glDrawArrays(GL_POINTS, 0, data.size());
}
