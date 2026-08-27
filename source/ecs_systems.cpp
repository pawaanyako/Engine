#include <ecs_systems.h>
#include <scene.h>

using AllComponents = std::tuple<
    NameComponent,
    TransformComponent,
    VelocityComponent,
    ParentComponent,
    ChildrenComponent,
    NodeComponent,
    MeshesComponent,
    MaterialsComponent,
    RenderableComponent>;

template <typename... Components>
std::string get_components(const entt::registry& registry, entt::entity entity) {
    std::string out = "Entity: " + std::to_string(static_cast<uint32_t>(entity)) + "\n";

    (void)((registry.any_of<Components>(entity)
                ? (out += "  - " + std::string(typeid(Components).name()) + "\n", 0)
                : 0),
           ...);

    return out;
}

template <typename... Components>
std::string get_all_components(const entt::registry& registry, entt::entity entity, std::tuple<Components...> const&) {
    return get_components<Components...>(registry, entity);
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

entt::entity EntityManagmentSystem::get_entity_by_name(const entt::registry& registry, const std::string& name) {
    entt::entity result = entt::null;
    const auto view = registry.view<NameComponent>();
    view.each([&](entt::entity entity, const NameComponent& name_component) {
        if (name_component.name == name) {
            result = entity;
        }
    });
    return result;
}

entt::entity EntityManagmentSystem::create_entity_and_node(entt::registry& registry,
                                                           const std::string& name,
                                                           Scene& scene,
                                                           entt::entity parent_entity) {
    entt::entity entity = registry.create();

    registry.emplace<NameComponent>(entity, name);
    registry.emplace<TransformComponent>(entity);
    registry.emplace<VelocityComponent>(entity);
    registry.emplace<ParentComponent>(entity, parent_entity);
    registry.emplace<ChildrenComponent>(entity);

    std::shared_ptr<Node> node = scene.add_node(name, scene.get_root_node(), {}, {});
    registry.emplace<NodeComponent>(entity, node.get()->get_name(), node);

    if (parent_entity != entt::null && registry.all_of<ChildrenComponent>(parent_entity)) {
        registry.get<ChildrenComponent>(parent_entity).children.push_back(entity);
    }

    Logger::debug("Entity ", registry.get<NameComponent>(entity).name, " was created");
    return entity;
}

entt::entity EntityManagmentSystem::create_entity_from_node(entt::registry& registry,
                                                            const Scene& scene,
                                                            const std::shared_ptr<Node>& node,
                                                            entt::entity parent_entity) {
    std::string node_name = node.get()->get_name();
    entt::entity entity = get_entity_by_name(registry, node_name);
    if (entity == entt::null) {
        entity = registry.create();

        registry.emplace<NameComponent>(entity, node_name);
        registry.emplace<TransformComponent>(entity);
        registry.emplace<VelocityComponent>(entity);
        registry.emplace<ParentComponent>(entity, parent_entity);
        registry.emplace<ChildrenComponent>(entity);

        if (parent_entity != entt::null && registry.all_of<ChildrenComponent>(parent_entity)) {
            registry.get<ChildrenComponent>(parent_entity).children.push_back(entity);
        }

        registry.emplace<NodeComponent>(entity, node.get()->get_name(), node);

        std::vector<GLuint> node_mesh_ids = node.get()->get_mesh_ids();
        if (!node_mesh_ids.empty()) {
            std::vector<std::string> mesh_names;
            std::vector<std::weak_ptr<Mesh>> meshes;
            for (auto&& node_mesh_id : node_mesh_ids) {
                std::shared_ptr<Mesh> node_mesh = get_mesh_by_id(scene, node_mesh_id);
                mesh_names.push_back(node_mesh.get()->get_name());
                meshes.push_back(node_mesh);
            }
            registry.emplace<MeshesComponent>(entity, mesh_names, meshes);
        }

        std::vector<GLuint> node_material_ids = node.get()->get_material_ids();
        if (!node_material_ids.empty()) {
            std::vector<std::string> material_names;
            std::vector<std::weak_ptr<Material>> materials;
            for (auto&& node_material_id : node_material_ids) {
                std::shared_ptr<Material> node_material = get_material_by_id(scene, node_material_id);
                material_names.push_back(node_material.get()->get_name());
                materials.push_back(node_material);
            }
            registry.emplace<MaterialsComponent>(entity, material_names, materials);
        }

        if (registry.all_of<MeshesComponent>(entity)) {
            registry.emplace<RenderableComponent>(entity, true, scene.get_shader());
        }
    } else {
        Logger::debug("Entity ", node_name, " already exists");
    }

    for (auto&& child_node : node->get_children()) {
        create_entity_from_node(registry, scene, child_node, entity);
    }
    Logger::debug("Entity ", registry.get<NameComponent>(entity).name, " was created");

    return entity;
};

std::string EntityManagmentSystem::get_entity_hierarchy_tree(const entt::registry& registry,
                                                             const entt::entity entity,
                                                             int depth) {
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

std::string EntityManagmentSystem::get_all_entity_components(const entt::registry& registry) {
    Logger::debug("EntityManagmentSystem get_all_entity_components");
    std::string out = "\n\n";

    const auto view = registry.view<NameComponent>();
    view.each([&](const entt::entity entity, const NameComponent& name_component) {
        out += get_all_components(registry, entity, AllComponents{});
    });
    return out;
}

glm::mat4 build_transform_matrix(const TransformComponent& transform_component) {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, transform_component.position);
    glm::vec3 rotation = glm::eulerAngles(transform_component.rotation);
    transform = glm::rotate(transform, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::rotate(transform, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::scale(transform, transform_component.scale);
    return transform;
}

glm::mat4 build_view_matrix(const TransformComponent& camera_transform, const CameraComponent& camera_component) {
    glm::vec3 forward = camera_transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_pos = camera_transform.position - forward * camera_component.radius;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::lookAt(camera_pos, camera_transform.position, up);
}

void EntityManagmentSystem::draw_all_meshes(const entt::registry& registry, int width, int height) {
    // Logger::debug("EntityManagmentSystem draw_all_meshes");
    const auto view_cameras = registry.view<CameraComponent>();

    const auto view = registry.view<TransformComponent, MeshesComponent, RenderableComponent>();
    view.each([&](const TransformComponent& transform_component,
                  const MeshesComponent& meshes_component,
                  const RenderableComponent& renderable_component) {
        if (!renderable_component.visible) {
            return;
        }

        entt::entity main_camera = get_entity_by_name(registry, "MainCamera");
        if (main_camera == entt::null) {
            Logger::error("Entity MainCamera was not found when draw_all_meshes");
            return;
        }
        TransformComponent main_camera_transform = registry.get<TransformComponent>(main_camera);
        CameraComponent main_camera_camera = registry.get<CameraComponent>(main_camera);

        std::shared_ptr<Shader> shader = renderable_component.shader.lock();
        shader.get()->use();

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(main_camera_camera.fov),
                                      (float)width / (float)height,
                                      main_camera_camera.near_plane,
                                      main_camera_camera.far_plane);
        GLint projection_location = glGetUniformLocation(shader.get()->get_program_id(), "projection");
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = build_view_matrix(main_camera_transform, main_camera_camera);
        GLint view_location = glGetUniformLocation(shader.get()->get_program_id(), "view");
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = build_transform_matrix(transform_component);
        GLint model_location = glGetUniformLocation(shader.get()->get_program_id(), "model");
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

        for (auto&& mesh : meshes_component.meshes) {
            mesh.lock().get()->draw(shader);
        }
    });
}

void EntityTransformSystem::set_entity_position(entt::registry& registry, const std::string& name, const glm::vec3& position) {
    entt::entity entity = EntityManagmentSystem::get_entity_by_name(registry, name);
    if (entity == entt::null) {
        Logger::error("Entity ", name, " was not found when set_entity_position");
        return;
    }
    set_entity_position(registry, entity, position);
}

void EntityTransformSystem::set_entity_position(entt::registry& registry, entt::entity entity, const glm::vec3& position) {
    TransformComponent& transfom_component = registry.get<TransformComponent>(entity);
    transfom_component.position = position;
    Logger::debug("Entity ", registry.get<NameComponent>(entity).name, " was positioned: ", glm::to_string(position));
}

void EntityTransformSystem::set_entity_rotation(entt::registry& registry, const std::string& name, const glm::vec3& rotation) {
    entt::entity entity = EntityManagmentSystem::get_entity_by_name(registry, name);
    if (entity == entt::null) {
        Logger::error("Entity ", name, " was not found when set_entity_rotation");
        return;
    }
    set_entity_rotation(registry, entity, rotation);
}

void EntityTransformSystem::set_entity_rotation(entt::registry& registry, entt::entity entity, const glm::vec3& rotation) {
    TransformComponent& transfom_component = registry.get<TransformComponent>(entity);
    glm::vec3 rotation_radians = glm::radians(rotation);
    glm::quat quat_z = glm::angleAxis(rotation_radians.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::quat quat_y = glm::angleAxis(rotation_radians.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat quat_x = glm::angleAxis(rotation_radians.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat new_rotation = quat_z * quat_y * quat_x;
    transfom_component.rotation = new_rotation;
    Logger::debug("Entity ", registry.get<NameComponent>(entity).name, " was rotated: ", glm::to_string(new_rotation));
}

void EntityTransformSystem::set_entity_scale(entt::registry& registry, const std::string& name, const glm::vec3& scale) {
    entt::entity entity = EntityManagmentSystem::get_entity_by_name(registry, name);
    if (entity == entt::null) {
        Logger::error("Entity ", name, " was not found when set_entity_rotation");
        return;
    }
    set_entity_scale(registry, entity, scale);
}

void EntityTransformSystem::set_entity_scale(entt::registry& registry, entt::entity entity, const glm::vec3& scale) {
    TransformComponent& transfom_component = registry.get<TransformComponent>(entity);
    transfom_component.scale = scale;
    Logger::debug("Entity ", registry.get<NameComponent>(entity).name, " was scaled: ", glm::to_string(scale));
}

void EntityMovementSystem::update_transform(entt::registry& registry, float frame_dutation) {
    const auto view = registry.view<TransformComponent, VelocityComponent>();
    view.each([&](TransformComponent& transform_component, const VelocityComponent& velocity_component) {
        transform_component.position += velocity_component.linear * frame_dutation;

        glm::vec3 omega = velocity_component.angular;
        glm::quat quat = transform_component.rotation;

        glm::quat omega_quat(0.0f, omega);
        glm::quat quat_dot = 0.5f * (omega_quat * quat);

        quat += quat_dot * frame_dutation;
        transform_component.rotation = glm::normalize(quat);

        transform_component.scale += velocity_component.scale_change * frame_dutation;
    });
}

void CameraSystem::update_camera_position(entt::registry& registry, const std::string& name, const glm::vec2& mouse_diff) {
    entt::entity camera = EntityManagmentSystem::get_entity_by_name(registry, name);
    if (camera == entt::null) {
        Logger::error("Entity ", name, " was not found when update_camera_position");
        return;
    }
    update_camera_position(registry, camera, mouse_diff);
}

void CameraSystem::update_camera_position(entt::registry& registry, entt::entity camera, const glm::vec2& mouse_diff) {
    TransformComponent& transform_component = registry.get<TransformComponent>(camera);
    CameraComponent& camera_component = registry.get<CameraComponent>(camera);

    glm::vec3 local_up = transform_component.rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 local_right = transform_component.rotation * glm::vec3(1.0f, 0.0f, 0.0f);

    transform_component.position += local_up * (mouse_diff.y * camera_component.linear_sensitivity) * camera_component.radius;
    transform_component.position += local_right * (-mouse_diff.x * camera_component.linear_sensitivity) * camera_component.radius;

    Logger::debug(registry.get<NameComponent>(camera).name, " new position = ", glm::to_string(transform_component.position));
}

void CameraSystem::update_camera_rotation(entt::registry& registry, const std::string& name, const glm::vec2& mouse_diff) {
    entt::entity camera = EntityManagmentSystem::get_entity_by_name(registry, name);
    if (camera == entt::null) {
        Logger::error("Entity ", name, " was not found when update_camera_rotation");
        return;
    }
    update_camera_rotation(registry, camera, mouse_diff);
}

void CameraSystem::update_camera_rotation(entt::registry& registry, entt::entity camera, const glm::vec2& mouse_diff) {
    TransformComponent& transform_component = registry.get<TransformComponent>(camera);
    CameraComponent& camera_component = registry.get<CameraComponent>(camera);

    camera_component.yaw -= glm::radians(mouse_diff.x * camera_component.angular_sensitivity);
    camera_component.pitch -= glm::radians(mouse_diff.y * camera_component.angular_sensitivity);
    camera_component.pitch = glm::clamp(camera_component.pitch, glm::radians(-89.9f), glm::radians(89.9f));

    glm::quat quat_yaw = glm::angleAxis(camera_component.yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat quat_pitch = glm::angleAxis(camera_component.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    transform_component.rotation = glm::normalize(quat_yaw * quat_pitch);

    Logger::debug(registry.get<NameComponent>(camera).name, " new rotation = ", glm::to_string(glm::degrees(glm::eulerAngles(transform_component.rotation))));
}