#ifndef CONTOUR_H
#define CONTOUR_H

#include "graphics/program.h"
#include "assets/obj.h"
#include "scene/global.h"
#include <QHash>

class Contour
{
public:
    Contour();
    Contour(GLFunctions *gl, Obj &mesh);
    void initialize(GLFunctions *gl, Obj &mesh);
    void draw();

    struct MeshBuffer {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    struct Adjacent {
        int face1 = -1; int vertex1 = -1;
        int face2 = -1; int vertex2 = -1;
        int getAdjVertex(int f) {
            Q_ASSERT_X(f == face1 || f == face2, "adj struct", "bad face request");
            if (f == face1)
                return vertex2;
            else
                return vertex1;
        }
        int getFace(int f) {
            if (f == face1)
                return face2;
            else
                return face1;
        }
    };

    glm::mat4 transform;
private:
    GLuint m_meshSize;

    GLFunctions *m_gl;
    Program m_goochFx;
    Program m_contourFx;
    GLuint m_bufferIndex;
    GLuint m_bufferAttribs;
    GLuint m_vao;
};

#endif // CONTOUR_H
