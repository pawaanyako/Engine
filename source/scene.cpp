#include <scene.h>

#include <filesystem>
#include <fstream>
#include <sstream>

std::vector<std::string> get_directories(const std::string& path) {
    std::vector<std::string> directories;

    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        Logger::error("Directory does not exist or is not a directory");
        return directories;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(entry.status())) {
            std::filesystem::path p = entry.path();
            p.make_preferred();
            directories.emplace_back(p.string());
        }
    }

    Logger::debug("Directories in directory \"", path, "\":");
    for (auto&& directory : directories) {
        Logger::debug(directory);
    }

    return directories;
}

std::vector<std::string> get_filepaths(const std::string& path) {
    std::vector<std::string> filepaths;

    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        Logger::error("Directory does not exist or is not a directory");
        return filepaths;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_regular_file(entry.status())) {
            std::filesystem::path p = entry.path();
            p.make_preferred();
            filepaths.emplace_back(p.string());
        }
    }

    Logger::debug("Files in directory \"", path, "\":");
    for (auto&& filename : filepaths) {
        Logger::debug(filename);
    }

    return filepaths;
}

void Scene::assimp_parse_obj_files(const std::string& models_directory_path) {
    Logger::debug("Pasring .obj files");
    std::vector<std::string> all_models_directories = get_directories(models_directory_path);
    for (auto&& model_directory : all_models_directories) {
        std::vector<std::string> filepaths = get_filepaths(model_directory);
        for (auto&& filepath : filepaths) {
            if (filepath.find("pigeon3.obj") != std::string::npos) {
                Assimp::Importer importer;
                unsigned int flags = aiProcess_CalcTangentSpace |
                                     aiProcess_JoinIdenticalVertices |
                                     aiProcess_Triangulate |
                                     aiProcess_GenSmoothNormals |
                                     aiProcess_SortByPType;
                Logger::debug("Assimp loading \"", filepath, "\"");
                const aiScene* ai_scene = importer.ReadFile(filepath, flags);
                if (!ai_scene) {
                    Logger::error("Assimp failed to load \"", filepath, "\": ", importer.GetErrorString());
                    continue;
                }
                if (!ai_scene->HasMeshes()) {
                    Logger::error("Assimp scene has no meshes");
                    continue;
                }
                if (!ai_scene->HasMaterials()) {
                    Logger::error("Assimp scene has no materials");
                    continue;
                }
                assimp_load_nodes(ai_scene, filepath);
            }
        }
    }
}

void Scene::assimp_load_nodes(const aiScene* ai_scene, const std::string& model_filepath) {
    Logger::debug("Loading nodes on scene");
    root_node_.get()->add_child(assimp_load_node(ai_scene, ai_scene->mRootNode, root_node_, model_filepath));
}

std::shared_ptr<Node> Scene::assimp_load_node(const aiScene* ai_scene,
                                              const aiNode* ai_node,
                                              const std::shared_ptr<Node>& parent_node,
                                              const std::string& model_filepath) {
    Logger::debug("Loading node ", ai_node->mName.C_Str(), " on scene");
    std::vector<GLuint> mesh_ids;
    std::vector<GLuint> material_ids;
    aiString ai_node_name = ai_node->mName;
    std::string node_name_string = ai_node_name.C_Str();
    for (size_t i = 0; i < ai_node->mNumMeshes; ++i) {
        aiMesh* ai_node_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
        aiMaterial* ai_node_material = ai_scene->mMaterials[ai_node_mesh->mMaterialIndex];

        aiString material_name;
        if (aiReturn_SUCCESS != ai_node_material->Get(AI_MATKEY_NAME, material_name)) {
            Logger::warn("Material has no name, using fallback");
            std::string model_name = std::filesystem::path(model_filepath).stem().string();
            for (size_t i = 0; i < ai_scene->mNumMaterials; ++i) {
                if (ai_scene->mMaterials[i] == ai_node_material) {
                    material_name = aiString(model_name + "_material_" + std::to_string(i));
                    break;
                }
            }
        }
        std::string material_name_string = std::string(material_name.C_Str());

        std::string mesh_name_string = node_name_string + "_" + material_name_string;
        std::shared_ptr<Mesh> mesh = assimp_load_mesh(ai_scene, ai_node_mesh, mesh_name_string, material_name_string, model_filepath);
        mesh_ids.push_back(mesh.get()->get_id());
        material_ids.push_back(mesh.get()->get_material_id());
    }
    std::shared_ptr<Node> node = std::make_shared<Node>(node_name_string, parent_node, mesh_ids, material_ids);
    Logger::debug("Loading node children, num_children = ", ai_node->mNumChildren);
    for (size_t i = 0; i < ai_node->mNumChildren; ++i) {
        node.get()->add_child(assimp_load_node(ai_scene, ai_node->mChildren[i], node, model_filepath));
    }
    return node;
}

std::shared_ptr<Mesh> Scene::assimp_load_mesh(const aiScene* ai_scene,
                                              const aiMesh* ai_mesh,
                                              const std::string& mesh_name_string,
                                              const std::string& material_name_string,
                                              const std::string& model_filepath) {
    Logger::debug("Loading mesh on scene");

    if (meshes_.find(mesh_name_string) != meshes_.end()) {
        Logger::debug("Mesh ", mesh_name_string, " is already loaded, skipping");
        return meshes_[mesh_name_string];
    }

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (size_t i = 0; i < ai_mesh->mNumVertices; ++i) {
        Vertex vertex;
        const aiVector3D& vertex_position = ai_mesh->mVertices[i];
        vertex.vertex_position = glm::vec3(vertex_position.x, vertex_position.y, vertex_position.z);
        if (ai_mesh->HasTextureCoords(0)) {
            const aiVector3D& vertex_texture = ai_mesh->mTextureCoords[0][i];
            vertex.vertex_texture = glm::vec2(vertex_texture.x, vertex_texture.y);
        } else {
            vertex.vertex_texture = glm::vec2(0.0f, 0.0f);
        }
        if (ai_mesh->HasNormals()) {
            const aiVector3D& vertex_normal = ai_mesh->mNormals[i];
            vertex.vertex_normal = glm::vec3(vertex_normal.x, vertex_normal.y, vertex_normal.z);
        } else {
            vertex.vertex_normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    for (size_t i = 0; i < ai_mesh->mNumFaces; i++) {
        aiFace face = ai_mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    std::shared_ptr<Material> material = assimp_load_material(ai_scene,
                                                              ai_scene->mMaterials[ai_mesh->mMaterialIndex],
                                                              material_name_string,
                                                              model_filepath);
    GLuint material_id = material.get()->get_id();

    meshes_[mesh_name_string] = std::make_shared<Mesh>(mesh_name_string,
                                                       vertices,
                                                       indices,
                                                       material_id);
    Logger::debug("Mesh ", mesh_name_string, " is loaded on scene");
    return meshes_[mesh_name_string];
}

std::shared_ptr<Material> Scene::assimp_load_material(const aiScene* ai_scene,
                                                      const aiMaterial* ai_material,
                                                      const std::string& material_name_string,
                                                      const std::string& model_filepath) {
    Logger::debug("Loading material on scene");

    auto convert_color = [](const aiColor3D& ai_color) {
        return glm::vec3(ai_color.r, ai_color.g, ai_color.b);
    };

    auto convert_type = [](const aiTextureType& ai_type) {
        switch (ai_type) {
        case aiTextureType_AMBIENT:
            return Ambient;
        case aiTextureType_DIFFUSE:
            return Diffuse;
        case aiTextureType_SPECULAR:
            return Specular;
        default:
            return None;
        };
    };

    auto get_textures_by_type = [&](const aiMaterial* ai_material, const aiTextureType& ai_type, const glm::vec3& color) {
        std::vector<Texture> textures;
        for (size_t i = 0; i < ai_material->GetTextureCount(ai_type); ++i) {
            Texture texture;
            texture.type = convert_type(ai_type);
            texture.color = color;
            aiString path;
            ai_material->GetTexture(ai_type, static_cast<unsigned int>(i), &path);
            texture.texture_map = (std::filesystem::path(model_filepath).parent_path() / path.C_Str()).string();
            textures.push_back(texture);
        }
        if (textures.empty()) {
            Texture texture;
            texture.type = convert_type(ai_type);
            texture.color = color;
            texture.texture_map.clear();
            textures.push_back(texture);
        }
        return textures;
    };

    if (materials_.find(material_name_string) != materials_.end()) {
        Logger::debug("Material ", material_name_string, " is already loaded, skipping");
        return materials_[material_name_string];
    }

    aiColor3D ka, kd, ks;
    if (aiReturn_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ka)) {
        ka = aiColor3D(0.0f);
    }
    if (aiReturn_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, kd)) {
        kd = aiColor3D(0.0f);
    }
    if (aiReturn_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ks)) {
        ks = aiColor3D(0.0f);
    }
    glm::vec3 color_ambient = convert_color(ka);
    glm::vec3 color_diffuse = convert_color(kd);
    glm::vec3 color_specular = convert_color(ks);

    std::vector<Texture> textures_ambient = get_textures_by_type(ai_material, aiTextureType_AMBIENT, color_ambient);
    std::vector<Texture> textures_diffuse = get_textures_by_type(ai_material, aiTextureType_DIFFUSE, color_diffuse);
    std::vector<Texture> textures_specular = get_textures_by_type(ai_material, aiTextureType_SPECULAR, color_specular);
    std::vector<Texture> all_textures;
    all_textures.insert(all_textures.end(), textures_ambient.begin(), textures_ambient.end());
    all_textures.insert(all_textures.end(), textures_diffuse.begin(), textures_diffuse.end());
    all_textures.insert(all_textures.end(), textures_specular.begin(), textures_specular.end());
    Logger::debug("All textures");
    for (auto&& texture : all_textures) {
        Logger::debug(texture);
    }

    float shininess, index_of_refraction, opacity;
    if (aiReturn_SUCCESS != ai_material->Get(AI_MATKEY_SHININESS, shininess)) {
        shininess = 0.0f;
    }
    if (aiReturn_SUCCESS != ai_material->Get(AI_MATKEY_REFRACTI, index_of_refraction)) {
        index_of_refraction = 1.0f;
    }
    if (aiReturn_SUCCESS != ai_material->Get(AI_MATKEY_OPACITY, opacity)) {
        opacity = 1.0f;
    }

    int shading_mode;
    if (AI_SUCCESS != ai_material->Get(AI_MATKEY_SHADING_MODEL, shading_mode)) {
        shading_mode = 1;
    }

    materials_[material_name_string] = std::make_shared<Material>(material_name_string, all_textures,
                                                                  shininess,
                                                                  index_of_refraction,
                                                                  opacity,
                                                                  shading_mode);
    Logger::debug("Material ", material_name_string, " is loaded on scene");
    return materials_[material_name_string];
}
