#include <mesh.h>

#include <fstream>
#include <sstream>

Mesh::Mesh(const std::vector<glm::vec3>& vertex_positions,
           const std::vector<glm::vec2>& vertex_textures,
           const std::vector<glm::vec3>& vertex_normals,
           const std::vector<GLuint>& vertex_indices) {
    Logger::debug("Mesh ctor(vertex_positions, vertex_textures, vertex_normals, vertex_indices)");
    generate_vertices(vertex_positions, vertex_textures, vertex_normals, {}, vertex_indices);
    bind_vao_vbo_ebo();
}

Mesh::Mesh(GLuint id,
           const std::string& name,
           const std::vector<Vertex>& vertices,
           const std::vector<GLuint>& vertex_indices,
           const std::shared_ptr<Material>& material) {
    Logger::debug("Mesh ctor(name, vertices, vertex_indices, material) - id: ", id, ", name: ", name);
    id_ = id;
    name_ = name;
    vertices_ = vertices;
    vertex_indices_ = vertex_indices;
    material_ = material;
    bind_vao_vbo_ebo();
}

Mesh::~Mesh() {
    Logger::debug("Mesh dtor()");
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
}

const GLuint Mesh::get_id() const {
    return id_;
}

const std::string& Mesh::get_name() const {
    return name_;
}

const std::weak_ptr<Material>& Mesh::get_material() const {
    return material_;
}

void Mesh::draw(const std::shared_ptr<Shader>& shader) const {
    glBindVertexArray(vao_);

    const std::vector textures = material_.lock().get()->get_textures();
    for (size_t i = 0; i < textures.size(); ++i) {
        if (textures[i].type == Diffuse) {
            glm::vec3 color = textures[i].color;
            GLint transformation_location = glGetUniformLocation(shader.get()->get_program_id(), "diffuse_color");
            glUniform3f(transformation_location, color.x, color.y, color.z);

            glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(i));
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
    }

    glDrawElements(GL_TRIANGLES, (GLsizei)vertex_indices_.size(), GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void Mesh::generate_vertices(const std::vector<glm::vec3>& vertex_positions,
                             const std::vector<glm::vec2>& vertex_textures,
                             const std::vector<glm::vec3>& vertex_normals,
                             const std::vector<Face>& faces,
                             const std::vector<GLuint>& vertex_indices) {
    if (!faces.empty()) {
        for (const auto& face : faces) {
            for (size_t i = 0; i < face.face_indices.size(); i += 3) {
                Vertex vertex = {
                    vertex_positions[face.face_indices[i + 0]],
                    vertex_textures[face.face_indices[i + 1]],
                    vertex_normals[face.face_indices[i + 2]]};
                vertices_.push_back(vertex);
                vertex_indices_.push_back(static_cast<GLuint>(vertices_.size() - 1));
            }
        }
    } else {
        for (size_t i = 0; i < vertex_positions.size(); ++i) {
            Vertex vertex = {
                vertex_positions[i],
                vertex_textures[i],
                vertex_normals[i]};
            vertices_.push_back(vertex);
        }
        vertex_indices_ = vertex_indices;
    }

    Logger::debug("Mesh generate_vertices - vertices_count: ", vertices_.size(),
                  " vertex_indices.size(): ", vertex_indices_.size(),
                  " triangle_count: ", (vertex_indices_.size() / 3));
}

void Mesh::bind_vao_vbo_ebo() {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices_.size() * sizeof(GLuint), vertex_indices_.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, vertex_position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, vertex_texture)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, vertex_normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    Logger::debug("Mesh bind_vao_vbo_ebo - vao: ", vao_, " vbo: ", vbo_, " ebo: ", ebo_);
}