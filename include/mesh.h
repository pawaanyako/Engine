#ifndef MESH_H_
#define MESH_H_

#include <config.h>
#include <maths.h>
#include <vertex.h>
#include <face.h>

class Mesh {
public:
    Mesh();
    Mesh(const std::string& filepath);
    Mesh(   std::vector<vec3<GLfloat>> vertex_positions,
            std::vector<vec2<GLfloat>> vertex_textures,
            std::vector<vec3<GLfloat>> vertex_normals,
            std::vector<GLuint> vertex_indices);
    ~Mesh();

    void generate_vao_vbo_ebo();
    void draw() const;

private:
    GLuint VAO_, VBO_, EBO_;
    std::vector<vec3<GLfloat>> vertex_positions_;
    std::vector<vec2<GLfloat>> vertex_textures_;
    std::vector<vec3<GLfloat>> vertex_normals_;
    std::vector<Face> faces_;
    std::vector<Vertex> vertices_;
    std::vector<GLuint> vertex_indices_;
    
    void parse_obj(const std::string& filepath);
    void generate_vertices();
};

#endif
