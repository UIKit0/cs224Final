#include "particles.h"

#include <QImage>

Particles::Particles(dWorldID w)
    : m_gl(NULL), world(w)
{
    // Create a space for this group of particles
    space = dHashSpaceCreate(0);
    dHashSpaceSetLevels(space, 1, 5);
}

Particles::Particles(GLFunctions *gl, int maxParticles)
{
    initialize(gl, maxParticles);
}

void Particles::initialize(GLFunctions *gl, int maxParticles)
{
    m_gl = gl;
    m_maxParticles = maxParticles;

    // Depth shader
//    m_depthPass.initialize(gl);
//    m_depthPass.compile(GL_VERTEX_SHADER,"depthPass.vertex.depth");
//    m_depthPass.compile(GL_FRAGMENT_SHADER,"depthPass.fragment.depth");
//    m_depthPass.link();

//    m_depthPosAttrib = m_depthPass.attrib("position");

    // Stencil shader
//    m_stencilPass.initialize(gl);
//    m_stencilPass.compile(GL_VERTEX_SHADER,"stencilPass.vertex.stencil");
//    m_stencilPass.compile(GL_GEOMETRY_SHADER,"stencilPass.geometry.stencil");
//    m_stencilPass.compile(GL_FRAGMENT_SHADER, "stencilPass.fragment.stencil");
//    m_stencilPass.link();

    // Smoke effects
    m_smokeFx.initialize(gl);
    m_smokeFx.compile(GL_VERTEX_SHADER, "smoke.vertex.billboard");
    m_smokeFx.compile(GL_GEOMETRY_SHADER, "smoke.geometry.billboard");
    m_smokeFx.compile(GL_FRAGMENT_SHADER, "smoke.fragment.billboard");
    m_smokeFx.link();

    m_pUniform = m_smokeFx.uniform("p_matrix");
    m_vUniform = m_smokeFx.uniform("v_matrix");
    m_mvUniform = m_smokeFx.uniform("mv_matrix");
    m_lightUniform = m_smokeFx.uniform("LightPosition");

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
    QImage* textures[texNum];
    textures[0] = new QImage(":/textures/smoke_alpha2.1.png");
    textures[1] = new QImage(":/textures/smoke_color2.png");
    textures[2] = new QImage(":/textures/smoke_depth2.png");
    textures[3] = new QImage(":/textures/smoke_normal2.png");

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
        if (textures[i]->isNull())
        {
            qCritical("Unable to load texture!");
            return;
        }

        QImage img = textures[i]->convertToFormat(QImage::Format_RGBA8888);

        m_gl->glActiveTexture(Tslots[i]);
        m_gl->glBindTexture(GL_TEXTURE_2D, m_texID[i]);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        delete textures[i];
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

    // Shadows!
    // 1. Render scene into depth buffer
    renderDepthPass();
    //      a. Bind pass through shader.
    //          i.   Vertex: Simple transform
    //          ii.  Fragment: Nothing
    //      b. Render geometry
    // 2. Render shadow volume into stencil buffer
    //glEnable(GL_STENCIL_TEST);
    renderStencilPass();
    //      a. Bind shadow volume shader
    //          i.   Vertex: Minor pass-thru
    //          ii.  Geometry: Quad emitter
    //          iii. Fragment: Nothing
    // 3. Render shadowed scene
    renderLightingPass();
    //glDisable(GL_STENCIL_TEST);
    //      a. Bind normal lighting shader
    //      b. Render geometry
}

void Particles::renderDepthPass()
{
//    m_gl->glDrawBuffers(GL_NONE);
//    m_gl->glDepthMask(GL_TRUE);

//    m_depthPass.use();

//    // Set projection matrix
//    m_gl->glUniformMatrix4fv(m_depthPass.uniform("MVP_Matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix * g_camera.vMatrix * g_model.mMatrix));

//    //Draw smoke
//    // Vertex attributes
//    quintptr offset = 0;
//    m_gl->glEnableVertexAttribArray(m_depthPass.attrib(""));
//    m_gl->glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);
//    offset += sizeof(glm::vec3);
//    m_gl->glEnableVertexAttribArray(m_sizeAttrib);
//    m_gl->glVertexAttribPointer(m_sizeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleBuffer), (const void *) offset);

//    glDrawArrays(GL_POINTS, 0, data.size());
}

void Particles::renderLightingPass()
{
//    m_gl->glEnable(GL_PROGRAM_POINT_SIZE);
    m_gl->glUseProgram(m_smokeFx.program());

    m_gl->glBindVertexArray(m_vao);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    m_gl->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleBuffer) * data.size(), data.data());

    m_gl->glUniformMatrix4fv(m_pUniform, 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
    m_gl->glUniformMatrix4fv(m_vUniform, 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix));
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

void Particles::renderStencilPass()
{

}
