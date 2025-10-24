#ifndef MESH_BUILDER_H_
#define MESH_BUILDER_H_

#include <config.h>
#include <mesh.h>

class MeshBuilder {
public:
    MeshBuilder();
    ~MeshBuilder();

    void add_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    void add_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec2 vt1, glm::vec2 vt2, glm::vec2 vt3);
    void add_quad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
    void add_quad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec2 vt1, glm::vec2 vt2, glm::vec2 vt3, glm::vec2 vt4);

    std::unique_ptr<Mesh> build();

private:
    std::vector<glm::vec3> vertex_positions_;
    std::vector<glm::vec2> vertex_textures_;
    std::vector<glm::vec3> vertex_normals_;
    std::vector<GLuint> vertex_indices_;
};

#endif
