#include <mesh_builder.h>

MeshBuilder::MeshBuilder() {
};

MeshBuilder::~MeshBuilder() {
};

void MeshBuilder::add_triangle(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3) {
    vec2<GLfloat> vt1 = {0.0f, 0.0f};
    vec2<GLfloat> vt2 = {1.0f, 0.0f};
    vec2<GLfloat> vt3 = {0.0f, 1.0f};
    this->add_triangle(v1, v2, v3, vt1, vt2, vt3);
}

void MeshBuilder::add_triangle(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3,
                               vec2<GLfloat> vt1, vec2<GLfloat> vt2, vec2<GLfloat> vt3) {
    GLuint count = (GLuint)vertex_positions_.size();

    vertex_positions_.push_back(v1);
    vertex_positions_.push_back(v2);
    vertex_positions_.push_back(v3);

    vertex_textures_.push_back(vt1);
    vertex_textures_.push_back(vt2);
    vertex_textures_.push_back(vt3);

    vec3<GLfloat> normal = ((v2 - v1).cross(v3 - v1)).normalized();
    vertex_normals_.push_back(normal);
    vertex_normals_.push_back(normal);
    vertex_normals_.push_back(normal);

    vertex_indices_.push_back(count + 0);
    vertex_indices_.push_back(count + 1);
    vertex_indices_.push_back(count + 2);
}

void MeshBuilder::add_quad(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3, vec3<GLfloat> v4) {
    vec2<GLfloat> vt1 = {0.0f, 0.0f};
    vec2<GLfloat> vt2 = {1.0f, 0.0f};
    vec2<GLfloat> vt3 = {1.0f, 1.0f};
    vec2<GLfloat> vt4 = {0.0f, 1.0f};
    this->add_quad(v1, v2, v3, v4, vt1, vt2, vt3, vt4);
}

void MeshBuilder::add_quad(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3, vec3<GLfloat> v4,
                           vec2<GLfloat> vt1, vec2<GLfloat> vt2, vec2<GLfloat> vt3, vec2<GLfloat> vt4) {
    GLuint count = (GLuint)vertex_positions_.size();

    vertex_positions_.push_back(v1);
    vertex_positions_.push_back(v2);
    vertex_positions_.push_back(v3);
    vertex_positions_.push_back(v4);

    vertex_textures_.push_back(vt1);
    vertex_textures_.push_back(vt2);
    vertex_textures_.push_back(vt3);
    vertex_textures_.push_back(vt4);

    vec3<GLfloat> normal = ((v2 - v1).cross(v4 - v1)).normalized();
    vertex_normals_.push_back(normal);
    vertex_normals_.push_back(normal);
    vertex_normals_.push_back(normal);
    vertex_normals_.push_back(normal);

    vertex_indices_.push_back(count + 0);
    vertex_indices_.push_back(count + 1);
    vertex_indices_.push_back(count + 2);
    vertex_indices_.push_back(count + 0);
    vertex_indices_.push_back(count + 2);
    vertex_indices_.push_back(count + 3);
}

const Mesh* MeshBuilder::build() {
    const Mesh* mesh = new Mesh(vertex_positions_, vertex_textures_, vertex_normals_, vertex_indices_);
    vertex_positions_.clear();
    vertex_textures_.clear();
    vertex_normals_.clear();
    vertex_indices_.clear();
    return mesh;
}
