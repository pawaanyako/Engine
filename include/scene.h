#pragma once

#include <config.h>
#include <material.h>
#include <mesh.h>
#include <node.h>

class Scene {
public:
    Scene::Scene() {
        Logger::debug("Scene ctor()");
        root_node_ = std::make_shared<Node>("RootNode", std::shared_ptr<Node>(), std::vector<GLuint>(), std::vector<GLuint>());
    }

    Scene::~Scene() {
        Logger::debug("Scene dtor()");
    }

    void Scene::set_shader(const std::shared_ptr<Shader>& shader) {
        shader_ = shader;
    }

    const std::shared_ptr<Shader>& Scene::get_shader() const {
        return shader_;
    }

    const std::unordered_map<std::string, std::shared_ptr<Mesh>>& Scene::get_meshes() const {
        return meshes_;
    }

    const std::unordered_map<std::string, std::shared_ptr<Material>>& Scene::get_materials() const {
        return materials_;
    }

    const std::shared_ptr<Node>& Scene::get_root_node() const {
        return root_node_;
    }

    std::shared_ptr<Node> add_node(const std::string& name,
                                   const std::shared_ptr<Node>& parent,
                                   const std::vector<GLuint>& mesh_ids,
                                   const std::vector<GLuint>& material_ids);

    void assimp_parse_obj_files(const std::string& models_directory_path);
    void assimp_load_nodes(const aiScene* ai_scene, const std::string& model_filepath);

private:
    std::shared_ptr<Shader> shader_;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
    std::shared_ptr<Node> root_node_;

    std::shared_ptr<Node> assimp_load_node(const aiScene* ai_scene,
                                           const aiNode* ai_node,
                                           const std::shared_ptr<Node>& parent_node,
                                           const std::string& model_filepath);
    std::shared_ptr<Mesh> assimp_load_mesh(const aiScene* ai_scene,
                                           const aiMesh* ai_mesh,
                                           const std::string& mesh_name_string,
                                           const std::string& material_name_string,
                                           const std::string& model_filepath);
    std::shared_ptr<Material> assimp_load_material(const aiScene* ai_scene,
                                                   const aiMaterial* ai_material,
                                                   const std::string& material_name_string,
                                                   const std::string& model_filepath);
};
