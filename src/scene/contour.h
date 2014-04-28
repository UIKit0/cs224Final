#ifndef CONTOUR_H
#define CONTOUR_H

#include "graphics/program.h"
#include "assets/obj.h"
#include "scene/global.h"

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
        int face1;
        int face2;
        int getFace(int f) {
            Q_ASSERT_X(f == face1 || f == face2, "adj struct", "bad face request");
            if (f == face1)
                return face1;
            else
                return face2;
        }
    };
private:
    GLuint m_meshSize;

    GLFunctions *m_gl;
    Program m_goochFx;
    GLuint m_buffer;
    GLuint m_vao;
};

#endif // CONTOUR_H
