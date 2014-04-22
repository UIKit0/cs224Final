#ifndef PROGRAM_H
#define PROGRAM_H

#include <QOpenGLFunctions_4_2_Core>
#include <QVector>
#include "glsw/glsw.h"
#include <QHash>
#include <QDebug>

/**
 * First outline of the program class. It's meant for the easy of use of shaders and
 * separable shaders.
 *
 * For now it uses glsw for wrangling a glsl file.
 *
 * Arthur
 */
class Program
{
public:
    Program();
    Program(QOpenGLFunctions_4_2_Core *gl, const char *path = "shaders");

    void initialize(QOpenGLFunctions_4_2_Core *gl, const char *path = "shaders");
    bool compile(GLenum shaderType, const char *name);
    void link();
    void use();

    GLuint program() const;
    GLint attrib(const char* name);
    GLint uniform(const char* name);

private:
    typedef GLint (*funLocator)(GLuint,const GLchar*);

    QOpenGLFunctions_4_2_Core *m_gl;
    GLuint m_program;
    QVector<GLuint> m_shaders;

    QHash<QByteArray,GLint> m_uniforms;
    QHash<QByteArray,GLint> m_attributes;
    void parseVariables(const QString &source, const QRegExp &search, QHash<QByteArray, GLint> &values);
    void setLocations(QHash<QByteArray, GLint> &values, bool unif);
};

#endif // PROGRAM_H
