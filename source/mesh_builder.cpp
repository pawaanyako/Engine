#include <mesh_builder.h>

MeshBuilder::MeshBuilder() {
};

MeshBuilder::~MeshBuilder() {
};

void MeshBuilder::add_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    glm::vec2 vt1 = {0.0f, 0.0f};
    glm::vec2 vt2 = {1.0f, 0.0f};
    glm::vec2 vt3 = {0.0f, 1.0f};
    this->add_triangle(v1, v2, v3, vt1, vt2, vt3);
}

void MeshBuilder::add_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
                               glm::vec2 vt1, glm::vec2 vt2, glm::vec2 vt3) {
    GLuint count = (GLuint)vertex_positions_.size();

    vertex_positions_.push_back(v1);
    vertex_positions_.push_back(v2);
    vertex_positions_.push_back(v3);

    vertex_textures_.push_back(vt1);
    vertex_textures_.push_back(vt2);
    vertex_textures_.push_back(vt3);

    glm::vec3 normal = glm::normalize((glm::cross((v2 - v1), (v3 - v1))));
    vertex_normals_.push_back(normal);
    vertex_normals_.push_back(normal);
    vertex_normals_.push_back(normal);

    vertex_indices_.push_back(count + 0);
    vertex_indices_.push_back(count + 1);
    vertex_indices_.push_back(count + 2);
}

void MeshBuilder::add_quad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) {
    glm::vec2 vt1 = {0.0f, 0.0f};
    glm::vec2 vt2 = {1.0f, 0.0f};
    glm::vec2 vt3 = {1.0f, 1.0f};
    glm::vec2 vt4 = {0.0f, 1.0f};
    this->add_quad(v1, v2, v3, v4, vt1, vt2, vt3, vt4);
}

void MeshBuilder::add_quad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4,
                           glm::vec2 vt1, glm::vec2 vt2, glm::vec2 vt3, glm::vec2 vt4) {
    GLuint count = (GLuint)vertex_positions_.size();

    vertex_positions_.push_back(v1);
    vertex_positions_.push_back(v2);
    vertex_positions_.push_back(v3);
    vertex_positions_.push_back(v4);

    vertex_textures_.push_back(vt1);
    vertex_textures_.push_back(vt2);
    vertex_textures_.push_back(vt3);
    vertex_textures_.push_back(vt4);

    glm::vec3 normal = glm::normalize((glm::cross((v2 - v1), (v4 - v1))));
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

std::shared_ptr<Mesh> MeshBuilder::build() {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertex_positions_, vertex_textures_, vertex_normals_, vertex_indices_);
    vertex_positions_.clear();
    vertex_textures_.clear();
    vertex_normals_.clear();
    vertex_indices_.clear();
    return mesh;
}
