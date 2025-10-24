#ifndef VERTEX_H_
#define VERTEX_H_

#include <config.h>
#include <maths.h>
#include <string>

struct Vertex {
    glm::vec3 vertex_position;
    glm::vec2 vertex_texture;
    glm::vec3 vertex_normal;

    std::string to_string(Vertex v) const {
        return "v:" + glm::to_string(v.vertex_position) + ", vt: " + glm::to_string(v.vertex_texture) + ", vn: " + glm::to_string(v.vertex_normal);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vertex& v) {
        os << "v: " << glm::to_string(v.vertex_position) << ", vt: " << glm::to_string(v.vertex_texture) << ", vn: " << glm::to_string(v.vertex_normal);
        return os;
    }
};

#endif
