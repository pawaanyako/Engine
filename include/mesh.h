#pragma once

#include <config.h>
#include <material.h>
#include <maths.h>
#include <shader.h>

struct Face {
    std::vector<GLuint> face_indices;
};

struct Vertex {
    glm::vec3 vertex_position;
    glm::vec2 vertex_texture;
    glm::vec3 vertex_normal;

    std::string to_string() const {
        return "v: " + glm::to_string(vertex_position) + ", vt: " + glm::to_string(vertex_texture) + ", vn: " + glm::to_string(vertex_normal);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vertex& v) {
        os << "v: " << glm::to_string(v.vertex_position) << ", vt: " << glm::to_string(v.vertex_texture) << ", vn: " << glm::to_string(v.vertex_normal);
        return os;
    }
};

class Mesh {
public:
    Mesh(const std::vector<glm::vec3>& vertex_positions,
         const std::vector<glm::vec2>& vertex_textures,
         const std::vector<glm::vec3>& vertex_normals,
         const std::vector<GLuint>& vertex_indices);
    Mesh(GLuint id,
         const std::string& name,
         const std::vector<Vertex>& vertices,
         const std::vector<GLuint>& vertex_indices,
         const std::shared_ptr<Material>& material);
    ~Mesh();

    const std::string& get_name() const;
    const GLuint get_id() const;
    const std::weak_ptr<Material>& get_material() const;

    void draw(const std::shared_ptr<Shader>& shader) const;

private:
    GLuint id_;
    std::string name_;
    GLuint vao_ = 0, vbo_ = 0, ebo_ = 0;
    std::vector<Vertex> vertices_;
    std::vector<GLuint> vertex_indices_;
    std::weak_ptr<Material> material_;

    void generate_vertices(const std::vector<glm::vec3>& vertex_positions,
                           const std::vector<glm::vec2>& vertex_textures,
                           const std::vector<glm::vec3>& vertex_normals,
                           const std::vector<Face>& faces,
                           const std::vector<GLuint>& vertex_indices);
    void bind_vao_vbo_ebo();
};
