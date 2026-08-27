#include <application.h>
#include <shader.h>
#include <time_manager.h>

void Application::init_glfw_private(int width, int height, const char* title) {
    Logger::debug("Application init_glfw_private(width, height, title)");

    if (is_initialized) {
        Logger::debug("Application is initialized");
        return;
    }

    if (!glfwInit()) {
        Logger::error("Failed to initialize GLFW");
        return;
    }
    Logger::debug("GLFW initialization success");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window_ == nullptr) {
        Logger::error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
    Logger::debug("GLFW create window success");
    glfwMakeContextCurrent(window_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::error("Failed to initialize GLAD");
        glfwTerminate();
        return;
    }
    Logger::debug("GLAD initialization success");

    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetKeyCallback(window_, key_callback);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetScrollCallback(window_, scroll_callback);
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    std::shared_ptr<Shader> shader = std::make_shared<Shader>(std::vector<std::string>{SHADERS_PATH "vertex.vert", SHADERS_PATH "fragment.frag"});
    scene_.set_shader(shader);

    TimeManager::set_target_fps(165.0f);
}

void Application::init_entt_private() {
    Logger::debug("Application init_glfw_private(width, height, title)");
    root_entity_ = EntityManagmentSystem::create_entity_from_node(registry_, scene_, scene_.get_root_node());

    entt::entity main_camera = EntityManagmentSystem::create_entity_and_node(registry_, "MainCamera", scene_, root_entity_);
    registry_.emplace<CameraComponent>(main_camera);
    // EntityTransformSystem::set_entity_rotation(registry_, main_camera, glm::vec3(-45.0f, 45.0f, 0.0f));
    // EntityTransformSystem::set_entity_scale(registry_, main_camera, glm::vec3(5.0f));

    scene_.assimp_parse_obj_files(MODELS_PATH);
    EntityManagmentSystem::create_entity_from_node(registry_, scene_, scene_.get_root_node());

    EntityTransformSystem::set_entity_position(registry_, "Cube.001", glm::vec3(0.0f, 0.5f, 0.0f));
    EntityTransformSystem::set_entity_position(registry_, "Suzanne", glm::vec3(0.0f, -0.5f, 0.0f));

    EntityTransformSystem::set_entity_rotation(registry_, "Cube.001", glm::vec3(0.0f, 0.0f, 0.0f));
    EntityTransformSystem::set_entity_rotation(registry_, "pigeon_Cube.001", glm::vec3(0.0f, 0.0f, 0.0f));
    EntityTransformSystem::set_entity_rotation(registry_, "Suzanne", glm::vec3(0.0f, 0.0f, 0.0f));

    EntityTransformSystem::set_entity_scale(registry_, "Cube.001", glm::vec3(1.0f / 16.0f));
    EntityTransformSystem::set_entity_scale(registry_, "pigeon_Cube.001", glm::vec3(1.0f / 16.0f));
    EntityTransformSystem::set_entity_scale(registry_, "Suzanne", glm::vec3(1.0f / 8.0f));

    EntityTransformSystem::set_entity_rotation(registry_, "GeoContainer", glm::vec3(-90.0f, 0.0f, 0.0f));
    EntityTransformSystem::set_entity_scale(registry_, "GeoContainer", glm::vec3(1.0f / 1000.0f));

    Logger::debug(EntityManagmentSystem::get_entity_hierarchy_tree(registry_, root_entity_));
    Logger::debug(EntityManagmentSystem::get_all_entity_components(registry_));
}

void Application::run_private() {
    Logger::debug("GLFW WindowShouldClose loop start");
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        process_input(window_);

        TimeManager::update();
        EntityMovementSystem::update_transform(registry_, TimeManager::get_frame_duration());
        render(window_);

        TimeManager::fps_limit();
    }
    Logger::debug("GLFW WindowShouldClose loop end");
}

void Application::process_input(GLFWwindow* window) {
    // if (keyboard_key_map_[GLFW_KEY_A]) {
    //     main_camera_velocity_component.linear.x = -CAMERA_LINEAR_VELOCITY;
    // }
    // if (keyboard_key_map_[GLFW_KEY_D]) {
    //     main_camera_velocity_component.linear.x = CAMERA_LINEAR_VELOCITY;
    // }
    // if (keyboard_key_map_[GLFW_KEY_W]) {
    //     main_camera_velocity_component.linear.y = CAMERA_LINEAR_VELOCITY;
    // }
    // if (keyboard_key_map_[GLFW_KEY_S]) {
    //     main_camera_velocity_component.linear.y = -CAMERA_LINEAR_VELOCITY;
    // }
    // if (keyboard_key_map_[GLFW_KEY_SPACE]) {
    //     main_camera_velocity_component.linear.z = CAMERA_LINEAR_VELOCITY;
    // }
    // if (keyboard_key_map_[GLFW_KEY_LEFT_SHIFT]) {
    //     main_camera_velocity_component.linear.z = -CAMERA_LINEAR_VELOCITY;
    // }
    // if (keyboard_key_map_[GLFW_KEY_E]) {
    //     main_camera_velocity_component.angular.y = -CAMERA_ANGULAR_VELOCITY;
    // }
    // if (keyboard_key_map_[GLFW_KEY_Q]) {
    //     main_camera_velocity_component.angular.y = CAMERA_ANGULAR_VELOCITY;
    // }

    if (mouse_button_map_[GLFW_MOUSE_BUTTON_2]) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glm::vec2 mouse_current_coordinates = glm::vec2(mouse_x, mouse_y);
        glm::vec2 mouse_diff = mouse_current_coordinates - mouse_last_coordinates_;
        CameraSystem::update_camera_rotation(registry_, "MainCamera", mouse_diff);
        mouse_last_coordinates_ = mouse_current_coordinates;
    }

    if (mouse_button_map_[GLFW_MOUSE_BUTTON_3]) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        glm::vec2 mouse_current_coordinates = glm::vec2(mouse_x, mouse_y);
        glm::vec2 mouse_diff = mouse_current_coordinates - mouse_last_coordinates_;
        CameraSystem::update_camera_position(registry_, "MainCamera", mouse_diff);
        mouse_last_coordinates_ = mouse_current_coordinates;
    }

    // if (!keyboard_key_map_[GLFW_KEY_A] && main_camera_velocity_component.linear.x < 0.0f) {
    //     main_camera_velocity_component.linear.x = 0.0f;
    // }
    // if (!keyboard_key_map_[GLFW_KEY_D] && main_camera_velocity_component.linear.x > 0.0f) {
    //     main_camera_velocity_component.linear.x = 0.0f;
    // }
    // if (!keyboard_key_map_[GLFW_KEY_W] && main_camera_velocity_component.linear.y > 0.0f) {
    //     main_camera_velocity_component.linear.y = 0.0f;
    // }
    // if (!keyboard_key_map_[GLFW_KEY_S] && main_camera_velocity_component.linear.y < 0.0f) {
    //     main_camera_velocity_component.linear.y = 0.0f;
    // }
    // if (!keyboard_key_map_[GLFW_KEY_SPACE] && main_camera_velocity_component.linear.z > 0.0f) {
    //     main_camera_velocity_component.linear.z = 0.0f;
    // }
    // if (!keyboard_key_map_[GLFW_KEY_LEFT_SHIFT] && main_camera_velocity_component.linear.z < 0.0f) {
    //     main_camera_velocity_component.linear.z = 0.0f;
    // }
    // if (!keyboard_key_map_[GLFW_KEY_E] && main_camera_velocity_component.angular.y < 0.0f) {
    //     main_camera_velocity_component.angular.y = 0.0f;
    // }
    // if (!keyboard_key_map_[GLFW_KEY_Q] && main_camera_velocity_component.angular.y > 0.0f) {
    //     main_camera_velocity_component.angular.y = 0.0f;
    // }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Application::render(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    EntityManagmentSystem::draw_all_meshes(registry_, width, height);

    glfwSwapBuffers(window_);
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    Application::render(window);
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0) {
        return;
    }
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        keyboard_key_map_[key] = true;
    } else if (action == GLFW_RELEASE) {
        keyboard_key_map_[key] = false;
    }

    // Logger::debug("Key = ", key, " action = ", action);
}

void Application::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button < 0) {
        return;
    }
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        mouse_button_map_[button] = true;
    } else if (action == GLFW_RELEASE) {
        mouse_button_map_[button] = false;
    }

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    mouse_last_coordinates_ = glm::vec2(mouse_x, mouse_y);

    // Logger::debug("Button = ", button, " action = ", action);
}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    entt::entity main_camera_entity = EntityManagmentSystem::get_entity_by_name(registry_, "MainCamera");
    if (main_camera_entity == entt::null) {
        Logger::error("Entity MainCamera was not found when scroll_callback");
        return;
    }

    CameraComponent& main_camera_camera_component = registry_.get<CameraComponent>(main_camera_entity);

    main_camera_camera_component.radius = glm::clamp(main_camera_camera_component.radius - static_cast<float>(yoffset), 1.0f, 10.0f);

    // Logger::debug("Scroll xoffset = ", xoffset, " yoffset = ", yoffset);
    Logger::debug("Camera new radius = ", main_camera_camera_component.radius);
}