#include <logger.h>
#include <scene.h>

#include <filesystem>
#include <fstream>
#include <sstream>

Scene::Scene() {
}

Scene::~Scene() {
    for (auto&& mesh : meshes_) {
        delete mesh;
    }
    for (auto&& material : materials_) {
        delete material;
    }
    Logger::debug("Scene is deleted");
}

const std::vector<Mesh*>& Scene::get_meshes() const {
    return meshes_;
}

const std::vector<Material*>& Scene::get_materials() const {
    return materials_;
}

void Scene::load_materials(const std::string& mtl_directory_path, const std::string& textures_directory_path) {
    Logger::debug("Loading all materials on scene");
    std::vector<std::string> filenames = get_file_names(mtl_directory_path);
    for (const std::string& filename : filenames) {
        if (filename.find(".mtl") != std::string::npos) {
            parse_mtl(mtl_directory_path + filename, textures_directory_path);
        }
    }
}

void Scene::load_meshes(const std::string& models_directory_path) {
    Logger::debug("Loading all meshes on scene");
    std::vector<std::string> filenames = get_file_names(models_directory_path);
    for (const std::string& filename : filenames) {
        if (filename.find(".obj") != std::string::npos) {
            parse_obj(models_directory_path + filename);
        }
    }
}

void Scene::draw_meshes(Shader* shader) const {
    for (auto&& mesh : meshes_) {
        mesh->draw(shader);
    }
}

std::vector<std::string> Scene::get_file_names(const std::string& path) const {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        throw std::runtime_error("Directory does not exist or is not a directory");
    }

    std::vector<std::string> filenames;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_regular_file(entry.status())) {
            filenames.push_back(entry.path().filename().string());
        }
    }

    Logger::debug("Files in directory \"" + path + "\":");
    for (const std::string& filename : filenames) {
        Logger::debug(filename);
    }

    return filenames;
}

void Scene::parse_mtl(const std::string& filepath, const std::string& textures_directory_path) {
    Logger::debug("Scene parse_mtl - filepath: \"" + filepath + "\"");

    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Failed to open mtl file: \"" + filepath + "\"");
    }

    auto load_current_material = [&](const std::string& name, const std::string& map_Kd) {
        if (name != "none") {
            std::string texture_path = (map_Kd != "none") ? textures_directory_path + map_Kd : map_Kd;
            load_material(new Material(name, texture_path));
        }
    };

    std::string line;
    std::string current_name = "none";
    std::string current_map_Kd = "none";

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string keyword;
        ss >> keyword;
        if (keyword.empty())
            continue;
        if (keyword == "newmtl") {
            load_current_material(current_name, current_map_Kd);
            ss >> current_name;
            current_map_Kd = "none";
        } else if (keyword == "map_Kd") {
            ss >> current_map_Kd;
        }
    }
    file.close();
    load_current_material(current_name, current_map_Kd);
}

void Scene::parse_obj(const std::string& filepath) {
    Logger::debug("Scene parse_obj - filepath: \"" + filepath + "\"");

    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Failed to open obj file: \"" + filepath + "\"");
    }

    auto parse_face_indices = [](const std::string& token) {
        std::vector<GLuint> indices;
        std::stringstream ss(token);
        std::string part;
        while (std::getline(ss, part, '/')) {
            if (!part.empty()) {
                indices.push_back(std::stoul(part));
            }
        }
        return indices;
    };

    std::string line;
    std::string current_name = "";
    std::string current_mtl_file;
    std::vector<vec3<GLfloat>> current_vertex_positions;
    std::vector<vec2<GLfloat>> current_vertex_textures;
    std::vector<vec3<GLfloat>> current_vertex_normals;
    std::string current_mtl_name;
    std::vector<Face> current_faces;
    SubMesh current_submesh;
    std::vector<SubMesh> current_submeshes;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string keyword;
        ss >> keyword;
        if (keyword.empty())
            continue;
        if (keyword == "mtllib") {
            ss >> current_mtl_file;
        } else if (keyword == "o") {
            if (current_name != "") {
                load_mesh(new Mesh(current_name,
                                   current_vertex_positions,
                                   current_vertex_textures,
                                   current_vertex_normals,
                                   current_faces,
                                   current_submeshes));
                current_vertex_positions.clear();
                current_vertex_textures.clear();
                current_vertex_normals.clear();
                current_faces.clear();
                current_submesh.index_start = 0;
                current_submesh.index_count = 0;
                current_submesh.materials.clear();
                current_submeshes.clear();
            }
            ss >> current_name;
        } else if (keyword == "v") {
            GLfloat vx, vy, vz;
            ss >> vx >> vy >> vz;
            current_vertex_positions.emplace_back(vx, vy, vz);
        } else if (keyword == "vt") {
            GLfloat u, v;
            ss >> u >> v;
            current_vertex_textures.emplace_back(u, v);
        } else if (keyword == "vn") {
            GLfloat nx, ny, nz;
            ss >> nx >> ny >> nz;
            current_vertex_normals.emplace_back(nx, ny, nz);
        } else if (keyword == "usemtl") {
            if (!current_submesh.materials.empty()) {
                current_submeshes.push_back(current_submesh);
                SubMesh new_submesh;
                GLuint accumulated = 0;
                for (auto&& submesh : current_submeshes) {
                    accumulated += submesh.index_count;
                }
                new_submesh.index_start = accumulated;
                current_submesh = std::move(new_submesh);
            }
            ss >> current_mtl_name;
            for (auto&& material : materials_) {
                if (material->get_name() == current_mtl_name) {
                    current_submesh.materials.push_back(material->get_id());
                }
            }
        } else if (keyword == "f") {
            Face face;
            std::string token;
            while (ss >> token) {
                std::vector<GLuint> indices = parse_face_indices(token);
                face.face_indices.insert(face.face_indices.end(), indices.begin(), indices.end());
            }
            GLuint face_vert_count = static_cast<GLuint>(face.face_indices.size()) / 3;
            current_submesh.index_count += (face_vert_count - 2) * 3;
            current_faces.push_back(face);
        }
    }
    file.close();
    current_submeshes.push_back(current_submesh);
    Logger::debug("current_submesh.index_start: " + std::to_string(current_submesh.index_start) +
                  " current_submesh.index_count: " + std::to_string(current_submesh.index_count) +
                  " current_submesh.materials[0]: " + std::to_string(current_submesh.materials[0]));
    load_mesh(new Mesh(current_name,
                       current_vertex_positions,
                       current_vertex_textures,
                       current_vertex_normals,
                       current_faces,
                       current_submeshes));
}

void Scene::load_mesh(Mesh* mesh) {
    meshes_.push_back(mesh);
    Logger::debug("Mesh is loaded on scene");
}

void Scene::load_material(Material* material) {
    materials_.push_back(material);
    Logger::debug("Material is loaded on scene");
}