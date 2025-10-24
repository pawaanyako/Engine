#ifndef MESH_H_
#define MESH_H_

#include <config.h>
#include <face.h>
#include <material.h>
#include <maths.h>
#include <shader.h>
#include <vertex.h>

struct SubMesh {
    GLuint index_start = 0;
    GLuint index_count = 0;
    std::vector<GLuint> materials;

    std::string to_string() const {
        std::string materials_str = "{ ";
        for (size_t i = 0; i < materials.size(); ++i) {
            materials_str += std::to_string(materials[i]);
            if (i + 1 < materials.size()) {
                materials_str += ", ";
            }
        }
        materials_str += " }";
        return "index_start: " + std::to_string(index_start) + ", index_count: " + std::to_string(index_count) + ", materials: " + materials_str;
    }
};

class Mesh {
public:
    Mesh();
    Mesh(std::vector<glm::vec3> vertex_positions,
         std::vector<glm::vec2> vertex_textures,
         std::vector<glm::vec3> vertex_normals,
         std::vector<GLuint> vertex_indices);
    Mesh(std::string name,
         std::vector<glm::vec3> vertex_positions,
         std::vector<glm::vec2> vertex_textures,
         std::vector<glm::vec3> vertex_normals,
         std::vector<Face> faces,
         std::vector<SubMesh> submeshes);
    ~Mesh();

    void draw(const std::unique_ptr<Shader>& shader) const;

private:
    std::string name_;
    GLuint VAO_ = 0, VBO_ = 0, EBO_ = 0;

    std::vector<glm::vec3> vertex_positions_;
    std::vector<glm::vec2> vertex_textures_;
    std::vector<glm::vec3> vertex_normals_;
    std::vector<Face> faces_;

    std::vector<Vertex> vertices_;
    std::vector<GLuint> vertex_indices_;

    std::vector<SubMesh> submeshes_;

    void generate_vertices();
    void bind_vao_vbo_ebo();
};

#endif
