#include "obj.h"
Obj::Obj()
{}
Obj::Obj(const QString &path)
{
    if (!read(path))
        qCritical() << "Unable to load OBJ: " << path;
}

void Obj::draw() const
{
    glBegin(GL_TRIANGLES);
    foreach (const Triangle &tri, triangles) {
        drawIndex(tri.a);
        drawIndex(tri.b);
        drawIndex(tri.c);
    }
    glEnd();
}

bool Obj::read(const QString &path)
{
    // Open the file
    QFile file(":/meshes/" + path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
    QTextStream f(&file);
    QString line;

    // Read the file
    QRegExp spaces("\\s+");
    do {
        line = f.readLine().trimmed();
        QStringList parts = line.split(spaces);
        if (parts.isEmpty()) continue;

        if (parts[0] == "v" && parts.count() >= 4) {
            vertices += glm::vec3(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat());
        } else if (parts[0] == "vt" && parts.count() >= 3) {
            coords += glm::vec2(parts[1].toFloat(), parts[2].toFloat());
        } else if (parts[0] == "vn" && parts.count() >= 4) {
            normals += glm::vec3(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat());
        } else if (parts[0] == "f" && parts.count() >= 4) {
            // Convert polygons into triangle fans
            Index a = getIndex(parts[1]);
            Index b = getIndex(parts[2]);
            for (int i = 3; i < parts.count(); i++) {
                Index c = getIndex(parts[i]);
                triangles += Triangle(a, b, c);
                b = c;
            }
        }
    } while (!line.isNull());

    return true;
}

static QString str(const glm::vec2 &v) { return QString("%1 %2").arg(v.x).arg(v.y); }
static QString str(const glm::vec3 &v) { return QString("%1 %2 %3").arg(v.x).arg(v.y).arg(v.z); }

static QString str(const Obj::Index &i)
{
    if (i.normal >= 0) {
        if (i.coord >= 0) return QString("%1/%2/%3").arg(i.vertex + 1).arg(i.coord + 1).arg(i.normal + 1);
        return QString("%1//%2").arg(i.vertex + 1).arg(i.normal + 1);
    } else {
        if (i.coord >= 0) return QString("%1/%2").arg(i.vertex + 1).arg(i.coord + 1);
        return QString("%1").arg(i.vertex + 1);
    }
}

bool Obj::write(const QString &path) const
{
    // Open the file
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) return false;
    QTextStream f(&file);

    // Write the file
    foreach (const glm::vec3 &vertex, vertices) f << "v " << str(vertex) << '\n';
    foreach (const glm::vec2 &coord, coords) f << "vt " << str(coord) << '\n';
    foreach (const glm::vec3 &normal, normals) f << "vn " << str(normal) << '\n';
    foreach (const Triangle &tri, triangles) f << "f " << str(tri.a) << ' ' << str(tri.b) << ' ' << str(tri.c) << '\n';

    return true;
}

inline int relativeIndex(int index, int count)
{
    return index >= 0 ? index - 1 : count + index;
}

Obj::Index Obj::getIndex(const QString &str) const
{
    QStringList parts = str.split('/');
    Index index;
    index.vertex = parts.count() > 0 ? relativeIndex(parts[0].toInt(), vertices.count()) : -1;
    index.coord = parts.count() > 1 ? relativeIndex(parts[1].toInt(), coords.count()) : -1;
    index.normal = parts.count() > 2 ? relativeIndex(parts[2].toInt(), normals.count()) : -1;
    return index;
}

void Obj::drawIndex(const Index &index) const
{
    if (index.coord >= 0 && index.coord < coords.count()) glTexCoord2fv(glm::value_ptr(coords[index.coord]));
    if (index.normal >= 0 && index.normal < normals.count()) glNormal3fv(glm::value_ptr(normals[index.normal]));
    if (index.vertex >= 0 && index.vertex < vertices.count()) glVertex3fv(glm::value_ptr(vertices[index.vertex]));
}
