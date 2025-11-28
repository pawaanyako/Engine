#include <ecs_systems.h>
#include <scene.h>

using AllComponents = std::tuple<
    NameComponent,
    TransformComponent,
    ParentComponent,
    ChildrenComponent,
    NodeComponent,
    MeshesComponent,
    MaterialsComponent,
    RenderableComponent>;

template <typename Tuple>
void print_all_components(const entt::registry& registry, entt::entity entity) {
    print_components_from_tuple_impl<Tuple>(registry, entity, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

template <typename Tuple, std::size_t... I>
void print_components_from_tuple_impl(const entt::registry& registry, entt::entity entity, std::index_sequence<I...>) {
    print_components<std::tuple_element_t<I, Tuple>...>(registry, entity);
}

template <typename... Components>
void print_components(const entt::registry& registry, entt::entity entity) {
    Logger::debug("Entity: ", (uint32_t)entity);

    ((registry.any_of<Components>(entity)
          ? Logger::debug("  - ", typeid(Components).name())
          : void()),
     ...);
}

std::shared_ptr<Mesh> get_mesh_by_id(const Scene& scene, GLuint id) {
    for (auto& [key, value] : scene.get_meshes()) {
        if (value->get_id() == id) {
            return value;
        }
    }
    return nullptr;
}

std::shared_ptr<Material> get_material_by_id(const Scene& scene, GLuint id) {
    for (auto& [key, value] : scene.get_materials()) {
        if (value->get_id() == id) {
            return value;
        }
    }
    return nullptr;
}

entt::entity EntityManagmentSystem::create_entity_from_node(entt::registry& registry,
                                                            const Scene& scene,
                                                            const std::shared_ptr<Node>& node,
                                                            entt::entity parent_entity) {
    entt::entity entity = registry.create();

    NameComponent name;
    name.name = node.get()->get_name();
    registry.emplace<NameComponent>(entity, std::move(name));

    TransformComponent transform;
    registry.emplace<TransformComponent>(entity, std::move(transform));

    ParentComponent parent;
    parent.parent = parent_entity;
    registry.emplace<ParentComponent>(entity, std::move(parent));

    ChildrenComponent children;
    registry.emplace<ChildrenComponent>(entity, std::move(children));

    if (parent_entity != entt::null && registry.all_of<ChildrenComponent>(parent_entity)) {
        registry.get<ChildrenComponent>(parent_entity).children.push_back(entity);
    }

    NodeComponent node_component;
    node_component.node_name = node.get()->get_name();
    node_component.node = node;
    registry.emplace<NodeComponent>(entity, std::move(node_component));

    std::vector<GLuint> node_mesh_ids = node.get()->get_mesh_ids();
    if (!node_mesh_ids.empty()) {
        MeshesComponent meshes_component;
        for (auto&& node_mesh_id : node_mesh_ids) {
            std::shared_ptr<Mesh> node_mesh = get_mesh_by_id(scene, node_mesh_id);
            meshes_component.mesh_names.push_back(node_mesh.get()->get_name());
            meshes_component.meshes.push_back(node_mesh);
        }
        registry.emplace<MeshesComponent>(entity, std::move(meshes_component));
    }

    std::vector<GLuint> node_material_ids = node.get()->get_material_ids();
    if (!node_material_ids.empty()) {
        MaterialsComponent material_component;
        for (auto&& node_material_id : node_material_ids) {
            std::shared_ptr<Material> node_material = get_material_by_id(scene, node_material_id);
            material_component.material_names.push_back(node_material.get()->get_name());
            material_component.materials.push_back(node_material);
        }
        registry.emplace<MaterialsComponent>(entity, std::move(material_component));
    }

    if (registry.all_of<MeshesComponent>(entity)) {
        RenderableComponent renderable_component;
        renderable_component.visible = true;
        renderable_component.shader = scene.get_shader();
        registry.emplace<RenderableComponent>(entity, std::move(renderable_component));
    }

    for (auto&& child_node : node->get_children()) {
        create_entity_from_node(registry, scene, child_node, entity);
    }
    Logger::debug("Entity ", registry.get<NameComponent>(entity).name, " was created");

    return entity;
};

std::string EntityManagmentSystem::get_entity_hierarchy_tree(const entt::registry& registry,
                                                             const entt::entity entity,
                                                             int depth) const {
    if (depth == 0) {
        Logger::debug("EntityManagmentSystem get_entity_hierarchy_tree");
    }

    const NameComponent& name_component = registry.get<NameComponent>(entity);
    std::string entity_tree_string;
    for (size_t i = 0; i < depth; i++) {
        entity_tree_string += " ";
    }
    entity_tree_string += "╚" + name_component.name + "\n";

    const ChildrenComponent& children_component = registry.get<ChildrenComponent>(entity);
    for (auto&& child_entity : children_component.children) {
        entity_tree_string += get_entity_hierarchy_tree(registry, child_entity, depth + 1);
    }

    if (depth == 0) {
        entity_tree_string = "\n\n" + entity_tree_string;
    }
    return entity_tree_string;
}

void EntityManagmentSystem::print_all_components_of_all_entities(const entt::registry& registry,
                                                                 const entt::entity entity) const {
    const NameComponent& name_component = registry.get<NameComponent>(entity);
    if (name_component.name == "RootNode") {
        Logger::debug("EntityManagmentSystem print_all_components_of_all_entities");
    }

    print_all_components<AllComponents>(registry, entity);

    const ChildrenComponent& children_component = registry.get<ChildrenComponent>(entity);
    for (auto&& child_entity : children_component.children) {
        print_all_components_of_all_entities(registry, child_entity);
    }
}

void EntityManagmentSystem::draw_all_meshes_of_all_entities(const entt::registry& registry,
                                                            const entt::entity entity) const {
    const NameComponent& name_component = registry.get<NameComponent>(entity);
    if (name_component.name == "RootNode") {
        // Logger::debug("EntityManagmentSystem draw_all_meshes_of_all_entities");
    }

    // const ChildrenComponent& children_component = registry.get<ChildrenComponent>(entity);
    // for (auto&& child_entity : children_component.children) {
    //     draw_all_meshes_of_all_entities(registry, child_entity);
    // }

    // if (registry.all_of<MeshesComponent>(entity)) {
    //     const MeshesComponent& meshes_component = registry.get<MeshesComponent>(entity);
    //     const RenderableComponent& renderable_component = registry.get<RenderableComponent>(entity);
    //     for (auto&& mesh : meshes_component.meshes) {
    //         mesh.lock().get()->draw(renderable_component.shader.lock());
    //     }
    // }

    auto view = registry.view<MeshesComponent, RenderableComponent>();
    view.each([](auto&& meshes_component, auto&& renderable_component) {
        for (auto&& mesh : meshes_component.meshes) {
            mesh.lock().get()->draw(renderable_component.shader.lock());
        }
    });
}