#define MATERIALS_PATH "../../resources/materials/"
#define MODELS_PATH "../../resources/models/"
#define SHADERS_PATH "../../resources/shaders/"
#define TEXTURES_PATH "../../resources/textures/"

#include <application.h>
#include <config.h>
#include <logger.h>
#include <mesh_builder.h>
#include <scene.h>
#include <shader.h>

#include <iostream>

Application::Application(int width, int height, const char* title) {
    Logger::debug("Application ctr(width, height, title)");
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
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
        throw std::runtime_error("Failed to create GLFW window");
        glfwTerminate();
    }
    Logger::debug("GLFW create window success");
    glfwMakeContextCurrent(window_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
        glfwTerminate();
    }
    Logger::debug("GLAD initialization success");

    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

Application::~Application() {
    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}

void Application::run() {
    scene_ = std::make_unique<Scene>();

    // MeshBuilder mesh_builder;
    // mesh_builder.add_quad({-0.5f, 0.5f, -1.0f},
    //                       {0.5f, 0.5f, -0.7f},
    //                       {0.5f, -0.5f, 0.3f},
    //                       {-0.5f, -0.5f, 1.0f});

    scene_->load_materials(MATERIALS_PATH, TEXTURES_PATH);
    scene_->load_meshes(MODELS_PATH);

    shader_ = std::make_unique<Shader>(std::vector<std::string>{SHADERS_PATH "vertex.vert", SHADERS_PATH "fragment.frag"});
    shader_->use();

    glm::mat4 transformation = glm::mat4(1.0f);
    transformation = glm::translate(transformation, glm::vec3(0.0f, 0.0f, 0.0f));
    transformation = glm::rotate(transformation, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float a = 2.0f;
    transformation = glm::scale(transformation, glm::vec3(1.0f / a, 1.0f / a, 1.0f / a));

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // limit fps
    const double TARGET_FPS = 60.0;
    const double FRAME_TIME = 1.0 / TARGET_FPS;
    double lastFrameTime = 0.0;

    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastFrameTime;

    Logger::debug("GLFW WindowShouldClose loop start");
    while (!glfwWindowShouldClose(window_)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        if (deltaTime < FRAME_TIME) {
            glfwWaitEventsTimeout(FRAME_TIME - deltaTime);
        }
        lastFrameTime = currentTime;
        // end limit fps

        transformation = glm::rotate(transformation, (GLfloat)deltaTime * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        GLuint transformation_location = glGetUniformLocation(shader_->get_program_id(), "transformation");
        glUniformMatrix4fv(transformation_location, 1, GL_FALSE, glm::value_ptr(transformation));

        render(window_);
        glfwPollEvents();
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

    process_input(window);
    scene_->draw_meshes(shader_);

    glfwSwapBuffers(window);
}
