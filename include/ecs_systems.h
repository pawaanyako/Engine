#pragma once

#include <ecs_components.h>
#include <scene.h>

class EntityManagmentSystem {
public:
    static entt::entity get_entity_by_name(const entt::registry& registry, const std::string& name);

    static entt::entity create_entity_and_node(entt::registry& registry,
                                               const std::string& name,
                                               Scene& scene,
                                               entt::entity parent_entity = entt::null);

    static entt::entity create_entity_from_node(entt::registry& registry,
                                                const Scene& scene,
                                                const std::shared_ptr<Node>& node,
                                                entt::entity parent_entity = entt::null);

    static std::string get_entity_hierarchy_tree(const entt::registry& registry, const entt::entity entity, int depth = 0);
    static std::string get_all_entity_components(const entt::registry& registry);

    static void draw_all_meshes(const entt::registry& registry, int width, int height);
};

class EntityTransformSystem {
public:
    static void set_entity_position(entt::registry& registry, const std::string& name, const glm::vec3& position);
    static void set_entity_position(entt::registry& registry, entt::entity entity, const glm::vec3& position);

    static void set_entity_rotation(entt::registry& registry, const std::string& name, const glm::vec3& rotation);
    static void set_entity_rotation(entt::registry& registry, entt::entity entity, const glm::vec3& rotation);

    static void set_entity_scale(entt::registry& registry, const std::string& name, const glm::vec3& scale);
    static void set_entity_scale(entt::registry& registry, entt::entity entity, const glm::vec3& scale);
};

class EntityMovementSystem {
public:
    static void update_transform(entt::registry& registry, float frame_dutation);
};

class CameraSystem {
public:
    static void create_camera(entt::registry& registry);

    static void update_camera_position(entt::registry& registry, const std::string& name, const glm::vec2& mouse_diff);
    static void update_camera_position(entt::registry& registry, entt::entity camera, const glm::vec2& mouse_diff);

    static void update_camera_rotation(entt::registry& registry, const std::string& name, const glm::vec2& mouse_diff);
    static void update_camera_rotation(entt::registry& registry, entt::entity camera, const glm::vec2& mouse_diff);
};
