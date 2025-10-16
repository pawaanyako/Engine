#include <logger.h>
#include <mesh.h>

#include <fstream>
#include <sstream>

Mesh::Mesh() {
    Logger::debug("Mesh ctr()");
};

// Mesh::Mesh(const std::string& filepath) {
//     Logger::debug("Mesh ctr(filepath) \"" + filepath + "\"");
//     VAO_ = 0;
//     VBO_ = 0;
//     EBO_ = 0;
//     if (filepath.find(".obj") != std::string::npos) {
//         parse_obj(filepath);
//         generate_vertices();
//         bind_vao_vbo_ebo();
//     }
// };

Mesh::Mesh(std::vector<vec3<GLfloat>> vertex_positions,
           std::vector<vec2<GLfloat>> vertex_textures,
           std::vector<vec3<GLfloat>> vertex_normals,
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
           std::vector<vec3<GLfloat>> vertex_positions,
           std::vector<vec2<GLfloat>> vertex_textures,
           std::vector<vec3<GLfloat>> vertex_normals,
           std::vector<Face> faces,
           std::vector<SubMesh> submeshes) {
    Logger::debug("Mesh ctr(string, vector, vector, vector, vector faces, vector) - name: " + name);
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

void Mesh::draw(Shader* shader) const {
    glBindVertexArray(VAO_);
    for (auto&& submesh : submeshes_) {
        glBindTexture(GL_TEXTURE_2D, submesh.materials[0]);
        glDrawElements(GL_TRIANGLES, (GLsizei)submesh.index_count, GL_UNSIGNED_INT, (const void*)(uintptr_t)(submesh.index_start * sizeof(GLuint)));
    }
    // glDrawElements(GL_TRIANGLES, (GLsizei)vertex_indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::set_materials(std::vector<Material*> materials) {
    Logger::debug("Mesh set_materials - submeshes size: " + std::to_string(submeshes_.size()) + " materials size: " + std::to_string(materials.size()));
    for (auto&& submesh : submeshes_) {
        for (auto&& material : materials) {
            submesh.materials.push_back(material->get_id());
        }
    }
}

// void Mesh::parse_obj(const std::string& filepath) {
//     Logger::debug("Mesh parse_obj - name = " + name_);

//     std::ifstream file(filepath);
//     if (!file) {
//         throw std::runtime_error("Failed to open obj file: " + filepath);
//     }

//     auto parse_face_indices = [](const std::string& token) {
//         std::vector<GLuint> indices;
//         std::stringstream ss(token);
//         std::string part;
//         while (std::getline(ss, part, '/')) {
//             if (!part.empty())
//                 indices.push_back(std::stoul(part));
//         }
//         return indices;
//     };

//     std::string line;
//     while (std::getline(file, line)) {
//         std::stringstream ss(line);
//         std::string keyword;
//         ss >> keyword;
//         if (keyword.empty())
//             continue;

//         if (keyword == "mtllib") {
//             std::string mtl_file;
//             ss >> mtl_file;
//             Logger::debug("mtl_file = " + mtl_file);
//         } else if (keyword == "o") {
//             ss >> name_;
//         } else if (keyword == "v") {
//             GLfloat x, y, z;
//             ss >> x >> y >> z;
//             vertex_positions_.emplace_back(x, y, z);
//         } else if (keyword == "vt") {
//             GLfloat u, v;
//             ss >> u >> v;
//             vertex_textures_.emplace_back(u, v);
//         } else if (keyword == "vn") {
//             GLfloat nx, ny, nz;
//             ss >> nx >> ny >> nz;
//             vertex_normals_.emplace_back(nx, ny, nz);
//         } else if (keyword == "usemtl") {
//             std::string material;
//             ss >> material;
//             Logger::debug("usemtl = " + material);
//         } else if (keyword == "f") {
//             Face face;
//             std::string vert;
//             while (ss >> vert) {
//                 std::vector<GLuint> indices = parse_face_indices(vert);
//                 face.face_indices.insert(face.face_indices.end(), indices.begin(), indices.end());
//             }
//             faces_.push_back(face);
//         }
//     }
//     file.close();
// }

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