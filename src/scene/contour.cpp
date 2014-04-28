#include "contour.h"

Contour::Contour()
    : m_gl(NULL)
{
}

Contour::Contour(GLFunctions *gl, Obj &mesh)
{
    initialize(gl, mesh);
}

void Contour::initialize(GLFunctions *gl, Obj &mesh)
{
    m_gl = gl;

    m_goochFx.initialize(gl, "../res/shaders/");
    m_goochFx.compile(GL_VERTEX_SHADER, "contour.vertex");
    m_goochFx.compile(GL_FRAGMENT_SHADER, "contour.fragment");
//    m_goochFx.compile(GL_GEOMETRY_SHADER, "contour.geometry");
    m_goochFx.link();

//    QHash<QPair<int,int>, Adjacent> edgeMap;
//    for(Obj::Triangle tri : mesh.triangles) {
//        for (int i = 0; i < 3; ++i) {
//            int ia = tri.indices[i].vertex;
//            int ib = tri.indices[(i + 1) % 3].vertex;
//            QPair<int, int> p(qMin(ia, ib), qMax(ia, ib));
//            if (!edgeMap.contains(p)) {
//                edgeMap.insert(p, )
//            } else {

//            }
//        }
//    }

    m_meshSize = mesh.triangles.size() * 3;
    QVector<MeshBuffer> data;
    data.reserve(m_meshSize);

    // TODO : add texture coords
    for(Obj::Triangle tri : mesh.triangles) {
        MeshBuffer a;
        a.position = mesh.vertices[tri.a.vertex];
        a.normal = mesh.normals[tri.a.normal];
        MeshBuffer b;
        b.position = mesh.vertices[tri.b.vertex];
        b.normal = mesh.normals[tri.b.normal];
        MeshBuffer c;
        c.position = mesh.vertices[tri.c.vertex];
        c.normal = mesh.normals[tri.c.normal];

        data.append(a);
        data.append(b);
        data.append(c);
    }

    gl->glGenVertexArrays(1, &m_vao);
    gl->glBindVertexArray(m_vao);

    gl->glGenBuffers(1, &m_buffer);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(MeshBuffer) * data.size(), data.data(), GL_STATIC_DRAW);

    quintptr offset = 0;

    gl->glEnableVertexAttribArray(m_goochFx.attrib("position"));
    gl->glVertexAttribPointer(m_goochFx.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);

    offset += sizeof(glm::vec3);

    gl->glEnableVertexAttribArray(m_goochFx.attrib("normal"));
    gl->glVertexAttribPointer(m_goochFx.attrib("normal"), 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);

    offset += sizeof(glm::vec3);

//    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (void *)offsetof(MeshBuffer, texcoord));
//    gl->glEnableVertexAttribArray(m_goochFx.attrib("texcoord"));
}

void Contour::draw()
{
    m_gl->glBindVertexArray(m_vao);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

    m_gl->glUseProgram(m_goochFx.program());

    m_gl->glUniformMatrix4fv(m_goochFx.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
    m_gl->glUniformMatrix4fv(m_goochFx.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix));

    m_gl->glUniform3f(m_goochFx.uniform("lightPos"), 0.0f, 10.0f, 4.0f);
    m_gl->glUniform3f(m_goochFx.uniform("surfaceColor"), 0.4f, 0.75f, 0.75f);
    m_gl->glUniform3f(m_goochFx.uniform("warmColor"), 0.6f, 0.6f, 0.0f);
    m_gl->glUniform3f(m_goochFx.uniform("coolColor"), 0.0f, 0.1f, 0.6f);
    m_gl->glUniform1f(m_goochFx.uniform("diffuseWarm"), 0.45f);
    m_gl->glUniform1f(m_goochFx.uniform("diffuseCool"), 0.15f);

    m_gl->glDrawArrays(GL_TRIANGLES, 0, m_meshSize);
}
