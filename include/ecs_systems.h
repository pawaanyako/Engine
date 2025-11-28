#pragma once

#include <ecs_components.h>
#include <scene.h>

class EntityManagmentSystem {
public:
    EntityManagmentSystem::EntityManagmentSystem() {
        Logger::debug("EntityManagmentSystem ctor()");
    }

    EntityManagmentSystem::~EntityManagmentSystem() {
        Logger::debug("EntityManagmentSystem dtor()");
    }

    void EntityManagmentSystem::set_root_entity(entt::entity root_entity) {
        root_entity_ = root_entity;
    }

    const entt::entity EntityManagmentSystem::get_root_entity() const {
        return root_entity_;
    }

    entt::entity create_entity_from_node(entt::registry& registry,
                                         const Scene& scene,
                                         const std::shared_ptr<Node>& node,
                                         entt::entity parent_entity = entt::null);

    std::string get_entity_hierarchy_tree(const entt::registry& registry,
                                          const entt::entity entity,
                                          int depth) const;

    void print_all_components_of_all_entities(const entt::registry& registry,
                                              const entt::entity entity) const;

    void draw_all_meshes_of_all_entities(const entt::registry& registry,
                                         const entt::entity entity) const;

private:
    entt::entity root_entity_ = entt::null;
};