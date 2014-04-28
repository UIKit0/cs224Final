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
    m_goochFx.compile(GL_VERTEX_SHADER, "contour.vertex.debug2");
    m_goochFx.compile(GL_FRAGMENT_SHADER, "contour.fragment.debug");
//    m_goochFx.compile(GL_GEOMETRY_SHADER, "contour.geometry");
    m_goochFx.link();

    m_meshSize = mesh.triangles.size() * 3;

    QHash<QPair<int,int>, Adjacent> edgeMap;
    for(int i = 0; i < mesh.triangles.size(); ++i) {
        Obj::Triangle &tri = mesh.triangles[i];
        qDebug() << "face(" << i << ")" << tri.a.vertex << tri.b.vertex << tri.c.vertex;

        for (int j = 0; j < 3; ++j) {
            int ia = tri.indices[j].vertex;
            int ib = tri.indices[(j + 1) % 3].vertex;
            int ic = tri.indices[(j + 2) % 3].vertex;

            QPair<int, int> p(qMin(ia, ib), qMax(ia, ib));
            if (!edgeMap.contains(p)) {
                Adjacent adj;
                adj.face1 = i;
                adj.vertex1 = ic;
                edgeMap.insert(p, adj);
            } else {
                Adjacent &adj = edgeMap[p];
                Q_ASSERT(adj.face2 == -1);
                adj.face2 = i;
                adj.vertex2 = ic;
            }
        }
    }

    // generate triangle adjecency index
    QVector<GLushort> adjIndex;
    for(int i = 0; i < mesh.triangles.size(); ++i) {
        Obj::Triangle &tri = mesh.triangles[i];

        for (int j = 0; j < 3; ++j) {
            int ia = tri.indices[j].vertex;
            int ib = tri.indices[(j + 1) % 3].vertex;

            adjIndex.append(ia);

            QPair<int, int> p(qMin(ia, ib), qMax(ia, ib));
            if (edgeMap.contains(p)) {
                Adjacent &adj = edgeMap[p];
                Q_ASSERT(adj.face2 != -1);
                adjIndex.append(adj.getAdjVertex(i));
            }
        }
    }

    QDebug debug = qDebug();
    for(int v : adjIndex) {
       debug << v;
    }

    gl->glGenVertexArrays(1, &m_vao);
    gl->glBindVertexArray(m_vao);

    m_gl->glGenBuffers(1, &m_bufferAttribs);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_bufferAttribs);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(),
                     mesh.vertices.data(), GL_STATIC_DRAW);

    quintptr offset = 0;
    m_gl->glVertexAttribPointer(m_goochFx.attrib("position"), 3, GL_FLOAT, GL_FALSE,
                                sizeof(glm::vec3), (const void *) offset);
    m_gl->glEnableVertexAttribArray(m_goochFx.attrib("position"));

    m_gl->glGenBuffers(1, &m_bufferIndex);
    m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferIndex);
    m_gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * adjIndex.size(),
                       adjIndex.data(),GL_STATIC_DRAW);

    qDebug() << "mesh size:" << m_meshSize;

    // uniforms
//    m_goochFx.use();
//    m_gl->glUniform3f(m_goochFx.uniform("lightPos"), 0.0f, 10.0f, 4.0f);
//    m_gl->glUniform3f(m_goochFx.uniform("surfaceColor"), 0.4f, 0.75f, 0.75f);
//    m_gl->glUniform3f(m_goochFx.uniform("warmColor"), 0.6f, 0.6f, 0.0f);
//    m_gl->glUniform3f(m_goochFx.uniform("coolColor"), 0.0f, 0.1f, 0.6f);
//    m_gl->glUniform1f(m_goochFx.uniform("diffuseWarm"), 0.45f);
//    m_gl->glUniform1f(m_goochFx.uniform("diffuseCool"), 0.15f);
}

void Contour::draw()
{
    m_gl->glBindVertexArray(m_vao);
    m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferIndex);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_bufferAttribs);

    m_gl->glUseProgram(m_goochFx.program());

    m_gl->glUniformMatrix4fv(m_goochFx.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
    m_gl->glUniformMatrix4fv(m_goochFx.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix));

    m_gl->glDrawElements(GL_TRIANGLES_ADJACENCY, m_meshSize * 2, GL_UNSIGNED_SHORT, 0);
}
