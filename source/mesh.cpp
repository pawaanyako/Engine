#include <mesh.h>

#include <fstream>
#include <sstream>
#include <thread>

Mesh::Mesh() {
    VAO_ = 0;
    VBO_ = 0;
    EBO_ = 0;
};

Mesh::Mesh(const std::string& filepath) {
    VAO_ = 0;
    VBO_ = 0;
    EBO_ = 0;
    if (filepath.find(".obj") != std::string::npos) {
        parse_obj(filepath);
        generate_vertices();
    }
};

Mesh::Mesh(std::vector<vec3<GLfloat>> vertex_positions,
           std::vector<vec2<GLfloat>> vertex_textures,
           std::vector<vec3<GLfloat>> vertex_normals,
           std::vector<GLuint> vertex_indices) {
    VAO_ = 0;
    VBO_ = 0;
    EBO_ = 0;
    vertex_positions_ = vertex_positions;
    vertex_textures_ = vertex_textures;
    vertex_normals_ = vertex_normals;
    vertex_indices_ = vertex_indices;
    generate_vertices();
}

Mesh::~Mesh() {
    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
    }
    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
    }
    if (EBO_ != 0) {
        glDeleteBuffers(1, &EBO_);
    }
};

void Mesh::draw() const {
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, (GLsizei)vertex_indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::parse_obj(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Failed to open obj file: " + filepath);
    }
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> words;
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            words.push_back(word);
        }
        if (words.size() == 0)
            continue;
        if (words[0] == "v") {
            vertex_positions_.push_back({std::stof(words[1]), std::stof(words[2]), std::stof(words[3])});
        } else if (words[0] == "vt") {
            vertex_textures_.push_back({std::stof(words[1]), std::stof(words[2])});
        } else if (words[0] == "vn") {
            vertex_normals_.push_back({std::stof(words[1]), std::stof(words[2]), std::stof(words[3])});
        } else if (words[0] == "f") {
            Face face;
            for (auto&& word : words) {
                if (word == "f")
                    continue;
                else {
                    std::stringstream ss2(word);
                    std::string token;
                    char delimiter = '/';
                    while (std::getline(ss2, token, delimiter)) {
                        face.face_indices.push_back(std::stoul(token));
                    }
                }
            }
            faces_.push_back(face);
        }
    }
    file.close();
}

void Mesh::generate_vertices() {
    if (faces_.size() > 0) {
        for (auto&& face : faces_) {
            GLuint vertex_count = static_cast<GLuint>(vertices_.size());
            size_t face_size = face.face_indices.size() / 3;
            for (size_t i = 0; i < face_size; i++) {
                Vertex vertex = {
                    vertex_positions_[face.face_indices[i * 3 + 0] - 1],
                    vertex_textures_[face.face_indices[i * 3 + 1] - 1],
                    vertex_normals_[face.face_indices[i * 3 + 2] - 1]};
                vertices_.push_back(vertex);
            }
            if (face_size == 3) {
                vertex_indices_.push_back(vertex_count + 0);
                vertex_indices_.push_back(vertex_count + 1);
                vertex_indices_.push_back(vertex_count + 2);
            } else if (face_size == 4) {
                vertex_indices_.push_back(vertex_count + 0);
                vertex_indices_.push_back(vertex_count + 1);
                vertex_indices_.push_back(vertex_count + 2);
                vertex_indices_.push_back(vertex_count + 0);
                vertex_indices_.push_back(vertex_count + 2);
                vertex_indices_.push_back(vertex_count + 3);
            }
        }
    } else {
        for (size_t i = 0; i < vertex_positions_.size(); i++) {
            Vertex vertex = {
                vertex_positions_[i],
                vertex_textures_[i],
                vertex_normals_[i]};
            vertices_.push_back(vertex);
        }
    }
}

void Mesh::generate_vao_vbo_ebo() {
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
}