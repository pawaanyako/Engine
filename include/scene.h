#ifndef SCENE_H_
#define SCENE_H_

#include <config.h>
#include <material.h>
#include <mesh.h>

class Scene {
public:
    Scene();
    ~Scene();

    const std::vector<std::unique_ptr<Material>>& get_materials() const;
    const std::vector<std::unique_ptr<Mesh>>& get_meshes() const;

    void load_materials(const std::string& mtl_directory_path, const std::string& textures_directory_path);
    void load_meshes(const std::string& models_directory_path);

    void draw_meshes(const std::unique_ptr<Shader>& shader) const;

private:
    std::vector<std::unique_ptr<Material>> materials_;
    std::vector<std::unique_ptr<Mesh>> meshes_;

    std::vector<std::string> get_file_names(const std::string& directory_path) const;

    void parse_mtl(const std::string& filepath, const std::string& textures_directory_path);
    void parse_obj(const std::string& filepath);

    void load_material(std::unique_ptr<Material> material);
    void load_mesh(std::unique_ptr<Mesh> mesh);
};

#endif
