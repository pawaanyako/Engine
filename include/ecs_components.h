#pragma once

#include <config.h>
#include <material.h>
#include <mesh.h>
#include <node.h>

struct NameComponent {
    std::string name;
};

struct TransformComponent {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};

struct ParentComponent {
    entt::entity parent = entt::null;
};

struct ChildrenComponent {
    std::vector<entt::entity> children;
};

struct NodeComponent {
    std::string node_name;
    std::weak_ptr<Node> node;
};

struct MeshesComponent {
    std::vector<std::string> mesh_names;
    std::vector<std::weak_ptr<Mesh>> meshes;
};

struct MaterialsComponent {
    std::vector<std::string> material_names;
    std::vector<std::weak_ptr<Material>> materials;
};

struct RenderableComponent {
    bool visible = true;
    std::weak_ptr<Shader> shader;
};
