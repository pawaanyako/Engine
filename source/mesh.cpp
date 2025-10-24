#include <logger.h>
#include <mesh.h>

#include <fstream>
#include <sstream>

Mesh::Mesh() {
    Logger::debug("Mesh default ctr()");
};

Mesh::Mesh(std::vector<glm::vec3> vertex_positions,
           std::vector<glm::vec2> vertex_textures,
           std::vector<glm::vec3> vertex_normals,
           std::vector<GLuint> vertex_indices) {
    Logger::debug("Mesh ctr(vector, vector, vector, vector indices)");
    vertex_positions_ = vertex_positions;
    vertex_textures_ = vertex_textures;
    vertex_normals_ = vertex_normals;
    vertex_indices_ = vertex_indices;
    generate_vertices();
    bind_vao_vbo_ebo();
}

Mesh::Mesh(std::string name,
           std::vector<glm::vec3> vertex_positions,
           std::vector<glm::vec2> vertex_textures,
           std::vector<glm::vec3> vertex_normals,
           std::vector<Face> faces,
           std::vector<SubMesh> submeshes) {
    Logger::debug("Mesh ctr(name, vertex_positions, vertex_textures, vertex_normals, faces, submeshes) - name: " + name);
    name_ = name;
    vertex_positions_ = vertex_positions;
    vertex_textures_ = vertex_textures;
    vertex_normals_ = vertex_normals;
    faces_ = faces;
    submeshes_ = submeshes;
    generate_vertices();
    bind_vao_vbo_ebo();
}

Mesh::~Mesh() {
    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
        VAO_ = 0;
    }
    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
        VBO_ = 0;
    }
    if (EBO_ != 0) {
        glDeleteBuffers(1, &EBO_);
        EBO_ = 0;
    }
    Logger::debug("Mesh is deleted from scene");
};

void Mesh::draw(const std::unique_ptr<Shader>& shader) const {
    glBindVertexArray(VAO_);
    for (auto&& submesh : submeshes_) {
        glBindTexture(GL_TEXTURE_2D, submesh.materials[0]);
        glDrawElements(GL_TRIANGLES, (GLsizei)submesh.index_count, GL_UNSIGNED_INT, (const void*)(uintptr_t)(submesh.index_start * sizeof(GLuint)));
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::generate_vertices() {
    if (faces_.size() > 0) {
        for (auto&& face : faces_) {
            GLuint vertex_count = static_cast<GLuint>(vertices_.size());
            size_t face_vert_count = face.face_indices.size() / 3;
            for (size_t i = 0; i < face_vert_count; ++i) {
                Vertex vertex = {
                    vertex_positions_[face.face_indices[i * 3 + 0] - 1],
                    vertex_textures_[face.face_indices[i * 3 + 1] - 1],
                    vertex_normals_[face.face_indices[i * 3 + 2] - 1]};
                vertices_.push_back(vertex);
            }
            for (size_t i = 0; i < face_vert_count - 2; ++i) {
                vertex_indices_.push_back(vertex_count + static_cast<GLuint>(0));
                vertex_indices_.push_back(vertex_count + static_cast<GLuint>(i + 1));
                vertex_indices_.push_back(vertex_count + static_cast<GLuint>(i + 2));
            }
        }
    } else {
        for (size_t i = 0; i < vertex_positions_.size(); ++i) {
            Vertex vertex = {
                vertex_positions_[i],
                vertex_textures_[i],
                vertex_normals_[i]};
            vertices_.push_back(vertex);
        }
    }
    Logger::debug("Mesh generate_vertices - vertices_count: " + std::to_string(vertices_.size()) +
                  " vertex_indices.size(): " + std::to_string(vertex_indices_.size()) +
                  " triangle_count: " + std::to_string(vertex_indices_.size() / 3));
}

void Mesh::bind_vao_vbo_ebo() {
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices_.size() * sizeof(GLuint), vertex_indices_.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, vertex_position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, vertex_texture)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, vertex_normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    Logger::debug("Mesh bind_vao_vbo_ebo - VAO: " + std::to_string(VAO_) + " VBO: " + std::to_string(VBO_) + " EBO: " + std::to_string(EBO_));
}