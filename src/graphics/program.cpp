#include "program.h"

const QRegExp Program::rxUniform = QRegExp(
    "\\s*uniform\\s+"
    "(?:lowp|medp|highp\\s+)?"
    "\\w+\\s+"
    "(\\w+)"
);

const QRegExp Program::rxAttributes = QRegExp(
    "\\s*in\\s+"
    "(?:lowp|medp|highp\\s+)?"
    "\\w+\\s+"
    "(\\w+)"
);

Program::Program()
    : m_gl(0)
    , m_program(INVALID_PROGRAM)
{
}

Program::Program(GLFunctions *gl)
{
    initialize(gl);
}

void Program::initialize(GLFunctions *gl)
{
    m_gl = gl;
    m_program = INVALID_PROGRAM;

    glswInit();
    glswSetPath(SHADER_PATH, ".glsl");

    // TODO: don't hardcode the directives
    // TODO: create a directives method

    glswAddDirectiveToken("", "#version 410 core");
}

bool Program::compile(GLenum shaderType, const char *name)
{
    Q_ASSERT(m_gl != NULL);

    const GLchar *shaderSource = glswGetShader(name);
    if (!shaderSource) {
        qCritical() << "Critical:" << glswGetError();
        return false;
    }

    GLuint shader = m_gl->glCreateShader(shaderType);
    m_shaders << shader;

    // send shader source and compile
    m_gl->glShaderSource(shader, 1, &shaderSource, NULL);
    m_gl->glCompileShader(shader);

    // store uniforms for use after linking
    QString source(shaderSource);
    parseVariables(source, rxUniform, m_uniforms);
    if (shaderType == GL_VERTEX_SHADER)
        parseVariables(source, rxAttributes, m_attributes);

#ifdef DEBUG_OPENGL
    qDebug() << "--- Shader:" << name << "---";
//    qDebug() << "uniforms:" << list;
//    qDebug() << shaderSource;

    GLint log_length;
    m_gl->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    QByteArray log(log_length, ' ');
    m_gl->glGetShaderInfoLog(shader, log_length, NULL, (GLchar*) log.data());

    if (log_length > 1) {
        qDebug() << QString(log);
        Q_ASSERT_X(false, "shader compile", "Invalid Shader!");
    }
#endif

    return true;
}

void Program::link()
{
    Q_ASSERT(m_program == INVALID_PROGRAM);

    // create program
    m_program = m_gl->glCreateProgram();

    // attach shaders
    for(GLuint shader : m_shaders)
        m_gl->glAttachShader(m_program, shader);

    // link
    m_gl->glLinkProgram(m_program);

    // set uniforms location
    setLocations(m_uniforms, true);
    setLocations(m_attributes, false);
    qDebug() << "uniforms:" << m_uniforms;
    qDebug() << "attributes:" << m_attributes;

    // destroy and clear shaders
    for(GLuint shader : m_shaders)
        m_gl->glDeleteShader(shader);

    m_shaders.clear();

    //unload glsw
    glswShutdown();
}

void Program::setLocations(QHash<QByteArray,GLint> &values, bool unif)
{
    GLint loc;
    const QList<QByteArray> &names = values.keys();

    for(QByteArray name : names) {
        if (unif)
            loc = m_gl->glGetUniformLocation(m_program, name);
        else
            loc = m_gl->glGetAttribLocation(m_program, name);
        values[name] = loc;

        if (loc < 0)
            qWarning() << "Warning:" << "invalid shader variable" << name;
    }
}

void Program::parseVariables(const QString &source,
                             const QRegExp &search,
                             QHash<QByteArray, GLint> &values)
{
    int pos = 0;
    while ((pos = search.indexIn(source, pos)) != INVALID_VAR) {
        values.insert(search.cap(1).toLocal8Bit(), INVALID_VAR);
        pos += search.matchedLength();
    }
}

void Program::use()
{
    Q_ASSERT(m_program != INVALID_PROGRAM);
    m_gl->glUseProgram(m_program);
}

GLuint Program::program() const
{
    Q_ASSERT(m_program != INVALID_PROGRAM);
    return m_program;
}

GLint Program::attrib(const char *name)
{
    Q_ASSERT_X(m_attributes.contains(name) && m_attributes[name] != INVALID_VAR, "invalid attribute request" , name);
    return m_attributes[name];
}

GLint Program::uniform(const char *name)
{
    Q_ASSERT_X(m_uniforms.contains(name) && m_uniforms[name] != INVALID_VAR, "invalid uniform request", name);
    return m_uniforms[name];
}
