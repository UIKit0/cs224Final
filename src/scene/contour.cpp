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

    m_goochFx.initialize(gl);
    m_goochFx.compile(GL_VERTEX_SHADER, "contour.vertex");
    m_goochFx.compile(GL_FRAGMENT_SHADER, "contour.fragment");
    m_goochFx.link();

    m_contourFx.initialize(gl);
    m_contourFx.compile(GL_GEOMETRY_SHADER, "contour.geometry2");
    m_contourFx.compile(GL_VERTEX_SHADER, "contour.vertex.line");
    m_contourFx.compile(GL_FRAGMENT_SHADER, "contour.fragment.black");
    m_contourFx.link();

    m_meshSize = mesh.triangles.size() * 3;

    if (m_meshSize > USHRT_MAX)
        qCritical() << "Critical: mesh is too big!";

    QHash<QPair<int,int>, Adjacent> edgeMap;
    for(int i = 0; i < mesh.triangles.size(); ++i) {
        Obj::Triangle &tri = mesh.triangles[i];
//        qDebug() << "face(" << i << ")" << tri.a.vertex << tri.b.vertex << tri.c.vertex;

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

    QHash<QPair<int,int>, int> vertexId;
    QVector<MeshBuffer> data;
    data.reserve(m_meshSize);

    // TODO : add texture coords
    for(int i = 0; i < mesh.triangles.size(); ++i) {
        Obj::Triangle &tri = mesh.triangles[i];
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
//        qDebug() << "face" << i << tri.a.vertex;
        QPair<int, int> ap(i, tri.a.vertex);
        QPair<int, int> bp(i, tri.b.vertex);
        QPair<int, int> cp(i, tri.c.vertex);
        int index = data.size() - 1;
        vertexId.insert(ap, index - 2);
        vertexId.insert(bp, index - 1);
        vertexId.insert(cp, index);
    }

    // generate triangle adjecency index
    QVector<GLushort> adjIndex;
    for(int i = 0; i < mesh.triangles.size(); ++i) {
        Obj::Triangle &tri = mesh.triangles[i];

        for (int j = 0; j < 3; ++j) {
            int ia = tri.indices[j].vertex;
            int ib = tri.indices[(j + 1) % 3].vertex;

            adjIndex.append((i * 3) + j);

            QPair<int, int> p(qMin(ia, ib), qMax(ia, ib));
            if (edgeMap.contains(p)) {
                Adjacent &adj = edgeMap[p];
//                Q_ASSERT(adj.face2 != -1);
                if (adj.face2 != -1) {
                    QPair<int, int> cp(adj.getFace(i), adj.getAdjVertex(i));
                    adjIndex.append(vertexId[cp]);
                } else {
                    adjIndex.append((i * 3) + (j + 2) % 3);
                }
            }
        }
    }

    gl->glGenVertexArrays(1, &m_vao);
    gl->glBindVertexArray(m_vao);

    m_gl->glGenBuffers(1, &m_bufferAttribs);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_bufferAttribs);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(MeshBuffer) * data.size(),
                    data.data(), GL_STATIC_DRAW);

    quintptr offset = 0;

    gl->glVertexAttribPointer(m_goochFx.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);
    gl->glEnableVertexAttribArray(m_goochFx.attrib("position"));

    gl->glVertexAttribPointer(m_contourFx.attrib("position"), 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);
    gl->glEnableVertexAttribArray(m_contourFx.attrib("position"));

    offset += sizeof(glm::vec3);

    gl->glVertexAttribPointer(m_goochFx.attrib("normal"), 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (const void *) offset);
    gl->glEnableVertexAttribArray(m_goochFx.attrib("normal"));

    offset += sizeof(glm::vec3);

    //    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBuffer), (void *)offsetof(MeshBuffer, texcoord));
    //    gl->glEnableVertexAttribArray(m_goochFx.attrib("texcoord"));

    m_gl->glGenBuffers(1, &m_bufferIndex);
    m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferIndex);
    m_gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * adjIndex.size(),
                       adjIndex.data(),GL_STATIC_DRAW);

    qDebug() << "mesh size:" << m_meshSize;

//    // uniforms
    m_goochFx.use();
    m_gl->glUniform3f(m_goochFx.uniform("lightPos"), 0.0f, 10.0f, 4.0f);
    m_gl->glUniform3f(m_goochFx.uniform("surfaceColor"), 0.4f, 0.75f, 0.75f);
    m_gl->glUniform3f(m_goochFx.uniform("warmColor"), 0.6f, 0.6f, 0.0f);
    m_gl->glUniform3f(m_goochFx.uniform("coolColor"), 0.0f, 0.1f, 0.6f);
    m_gl->glUniform1f(m_goochFx.uniform("diffuseWarm"), 0.45f);
    m_gl->glUniform1f(m_goochFx.uniform("diffuseCool"), 0.15f);
}

void Contour::draw()
{
    m_gl->glBindVertexArray(m_vao);
    m_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferIndex);
    m_gl->glBindBuffer(GL_ARRAY_BUFFER, m_bufferAttribs);

//    m_gl->glUseProgram(m_goochFx.program());

//    m_gl->glUniformMatrix4fv(m_goochFx.uniform("proj_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix));
//    m_gl->glUniformMatrix4fv(m_goochFx.uniform("mv_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix * g_model.mMatrix));

//    m_gl->glDrawElements(GL_TRIANGLES_ADJACENCY, m_meshSize * 2, GL_UNSIGNED_SHORT, 0);

//    m_gl->glDisable(GL_DEPTH_TEST);
//    m_gl->glEnable(GL_BLEND);

    m_gl->glUseProgram(m_contourFx.program());

    m_gl->glUniformMatrix4fv(m_contourFx.uniform("mvp_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.pMatrix * g_camera.vMatrix * g_model.mMatrix));
    m_gl->glUniformMatrix4fv(m_contourFx.uniform("m_matrix"), 1, GL_FALSE, glm::value_ptr(g_model.mMatrix));
//    m_gl->glUniformMatrix4fv(m_contourFx.uniform("v_matrix"), 1, GL_FALSE, glm::value_ptr(g_camera.vMatrix));
    m_gl->glUniform3fv(m_contourFx.uniform("viewVec"), 1, glm::value_ptr(g_camera.m_lookAt));

    m_gl->glDrawElements(GL_TRIANGLES_ADJACENCY, m_meshSize * 2, GL_UNSIGNED_SHORT, 0);

//    m_gl->glDisable(GL_BLEND);
}
