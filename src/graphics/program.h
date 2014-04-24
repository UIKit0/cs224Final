#ifndef PROGRAM_H
#define PROGRAM_H

#include <QOpenGLFunctions_2_0>
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
    Program(QOpenGLFunctions_2_0 *gl, const char *path = "shaders");

    void initialize(QOpenGLFunctions_2_0 *gl, const char *path = "shaders");
    bool compile(GLenum shaderType, const char *name);
    void link();
    void use();

    GLuint program() const;
    GLint attrib(const char* name);
    GLint uniform(const char* name);

private:
    typedef GLint (*funLocator)(GLuint,const GLchar*);

    QOpenGLFunctions_2_0 *m_gl;
    GLuint m_program;
    QVector<GLuint> m_shaders;

    QHash<QByteArray,GLint> m_uniforms;
    QHash<QByteArray,GLint> m_attributes;
    void parseVariables(const QString &source, const QRegExp &search, QHash<QByteArray, GLint> &values);
    void setLocations(QHash<QByteArray, GLint> &values, bool unif);

    static const QRegExp rxUniform;
    static const QRegExp rxAttributes;
    static const GLuint INVALID_VAR = -1;
};

#endif // PROGRAM_H
