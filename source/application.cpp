#define MODELS_PATH "../../resources/models/"
#define SHADERS_PATH "../../resources/shaders/"
#include <config.h>
#include <iostream>

#include <application.h>
#include <mesh_builder.h>
#include <object.h>
#include <scene.h>
#include <shader.h>

MeshBuilder mesh_builder;
Scene scene;

Application::Application(int width, int height, const char* title) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to init GLFW");
    }

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
    glfwMakeContextCurrent(window_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
        glfwTerminate();
    }

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
    vec3<GLfloat> v1 = {-0.5f, 0.5f, -1.0f};
    vec3<GLfloat> v2 = {0.5f, 0.5f, -0.7f};
    vec3<GLfloat> v3 = {0.5f, -0.5f, 0.3f};
    vec3<GLfloat> v4 = {-0.5f, -0.5f, 1.0f};
    // mesh_builder.add_triangle(v1, v2, v3);
    mesh_builder.add_quad(v1, v2, v3, v4);

    // scene.load_mesh(mesh_builder.build());
    scene.load_mesh(Mesh(MODELS_PATH "suzanne.obj"));

    Shader shader({SHADERS_PATH "vertex.vert", SHADERS_PATH "fragment.frag"});
    shader.use();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // limit fps
    const double TARGET_FPS = 1.0;
    const double FRAME_TIME = 1.0 / TARGET_FPS;
    double lastFrameTime = 0.0;

    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastFrameTime;

    while (!glfwWindowShouldClose(window_)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        if (deltaTime < FRAME_TIME) {
            glfwWaitEventsTimeout(FRAME_TIME - deltaTime);
        }
        lastFrameTime = currentTime;
        // end limit fps

        render(window_);
        glfwPollEvents();
    }
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    render(window);
}

void Application::process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void Application::render(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);

    process_input(window);
    scene.draw_meshes();

    glfwSwapBuffers(window);
}