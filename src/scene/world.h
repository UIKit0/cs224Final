#ifndef WORLD_H
#define WORLD_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <QHash>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QImage>

#include "assets/obj.h"
#include "scene/camera.h"
#include "scene/global.h"
#include "graphics/program.h"

#include "interaction/vortex.h"
#include "interaction/solidobject.h"
#include "particles/smokeparticle.h"
#include "particles/basicsmokeemitter.h"
#include "particles/smoketrailemitter.h"
#include "scene/terrain.h"

extern void handleVortexCollision(Vortex *v, dBodyID pbody);
extern void handleWindVolumeCollision(WindVolume *v, dBodyID pbody);
extern QHash<dBodyID, Vortex*> g_vortices;

class World
{
public:
    World();
    virtual ~World();

    void initialize(GLFunctions *gl);
    void render(GLFunctions *gl);
    void update(float seconds);

    void setScreenSize(int width, int height);

    void keyReleaseEvent(QKeyEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseMoveDelta(float deltaX, float deltaY);

    struct MeshBuffer {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;

//        MeshBuffer() {}
//        MeshBuffer(glm::vec3 &pos, glm::vec3 &norm, glm::vec2 &tex) :
//            position(pos), normal(norm), texcoord(tex) {}
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

public:

    // PARTICLES
    void toggleDrawVortices();
    void toggleMovingSphere();

    // ODE stuff that is only created once per world
    dWorldID m_world_id;
    dSpaceID space;
    dMass m;
    dJointGroupID contactgroup;

    QList<ParticleEmitter*> emitters;

    ParticleEmitter *circlingEmitter;

    // Sphere that moves left and right through the smoke
    SolidObject sphere;
    Obj sphereMesh;

    bool moveSphere;
    bool moveWing;

    Terrain terrain;

private:
    Obj m_mesh;

    int m_screenWidth;
    int m_screenHeight;

    GLuint m_buffer;
    GLuint m_vao;
//    GLuint m_vboIds[2];

    Program m_goochFx;
    Program m_smokeFx;

    GLuint m_posAttr;
    GLuint m_texAttr;
    GLuint m_matrixUniform;
    GLuint m_textureUniform;
    GLuint m_texture;

    // Camera
//    Camera m_camera;
};

#endif // WORLD_H
