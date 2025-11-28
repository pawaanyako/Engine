#include <application.h>
#include <fps_limiter.h>
#include <shader.h>

Application::Application(int width, int height, const char* title) {
    Logger::debug("Application ctor(width, height, title)");
    if (!glfwInit()) {
        Logger::error("Failed to initialize GLFW");
        return;
    }
    Logger::debug("GLFW initialization success");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    std::shared_ptr<Shader> shader = std::make_shared<Shader>(std::vector<std::string>{SHADERS_PATH "vertex.vert", SHADERS_PATH "fragment.frag"});
    scene_.set_shader(shader);
    scene_.assimp_parse_obj_files(MODELS_PATH);
}

Application::~Application() {
    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
    Logger::debug("Application dtor()");
}

void Application::run() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    scene_.get_shader().get()->use();

    entt::entity root_entity = entity_managment_system_.create_entity_from_node(registry_, get_scene(), get_scene().get_root_node());
    entity_managment_system_.set_root_entity(root_entity);
    Logger::debug(entity_managment_system_.get_entity_hierarchy_tree(registry_, root_entity, 0));
    entity_managment_system_.print_all_components_of_all_entities(registry_, root_entity);

    float fps = 165.0f;
    FPSLimiter fps_limiter(fps);

    glm::mat4 transformation = glm::mat4(1.0f);
    transformation = glm::translate(transformation, glm::vec3(0.0f, 0.0f, 0.0f));
    // transformation = glm::rotate(transformation, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float a = 16.0f;
    transformation = glm::scale(transformation, glm::vec3(1.0f / a, 1.0f / a, 1.0f / a));

    Logger::debug("GLFW WindowShouldClose loop start");
    while (!glfwWindowShouldClose(window_)) {
        process_input(window_);

        float delta_time = 1.0f / fps;
        transformation = glm::rotate(transformation, delta_time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        GLint transformation_location = glGetUniformLocation(scene_.get_shader()->get_program_id(), "transformation");
        glUniformMatrix4fv(transformation_location, 1, GL_FALSE, glm::value_ptr(transformation));

        render(window_);

        glfwPollEvents();

        fps_limiter.limit();
    }
    Logger::debug("GLFW WindowShouldClose loop end");
}

void Application::framebuffer_size_callback(GLFWwindow* window, GLsizei width, GLsizei height) {
    glViewport(0, 0, width, height);
    Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    if (application != nullptr) {
        application->render(window);
    }
}

void Application::process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Application::render(GLFWwindow* window) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    entity_managment_system_.draw_all_meshes_of_all_entities(registry_, entity_managment_system_.get_root_entity());

    glfwSwapBuffers(window);
}