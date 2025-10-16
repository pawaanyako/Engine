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
};

class Mesh {
public:
    Mesh();
    // Mesh(const std::string& filepath);
    Mesh(std::vector<vec3<GLfloat>> vertex_positions,
         std::vector<vec2<GLfloat>> vertex_textures,
         std::vector<vec3<GLfloat>> vertex_normals,
         std::vector<GLuint> vertex_indices);
    Mesh(std::string name,
         std::vector<vec3<GLfloat>> vertex_positions,
         std::vector<vec2<GLfloat>> vertex_textures,
         std::vector<vec3<GLfloat>> vertex_normals,
         std::vector<Face> faces,
         std::vector<SubMesh> submeshes);
    ~Mesh();

    void draw(Shader* shader) const;
    void set_materials(std::vector<Material*> materials);

private:
    std::string name_;
    GLuint VAO_ = 0, VBO_ = 0, EBO_ = 0;

    std::vector<vec3<GLfloat>> vertex_positions_;
    std::vector<vec2<GLfloat>> vertex_textures_;
    std::vector<vec3<GLfloat>> vertex_normals_;
    std::vector<Face> faces_;

    std::vector<Vertex> vertices_;
    std::vector<GLuint> vertex_indices_;

    std::vector<SubMesh> submeshes_;

    // void parse_obj(const std::string& filepath);
    void generate_vertices();
    void bind_vao_vbo_ebo();
};

#endif
