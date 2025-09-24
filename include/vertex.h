#ifndef VERTEX_H_
#define VERTEX_H_

#include <config.h>
#include <maths.h>

struct Vertex {
    vec3<GLfloat> vertex_position;
    vec2<GLfloat> vertex_texture;
    vec3<GLfloat> vertex_normal;

    friend std::ostream& operator<<(std::ostream& os, const Vertex& v) {
        os << "v = " << v.vertex_position << ", vt = " << v.vertex_texture << ", vn = " << v.vertex_normal;
        return os;
    }
};

#endif
