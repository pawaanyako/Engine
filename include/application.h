#pragma once

#define MODELS_PATH "resources\\models\\"
#define SHADERS_PATH "resources\\shaders\\"

#define CAMERA_LINEAR_VELOCITY 1.0f
#define CAMERA_ANGULAR_VELOCITY 1.0f

#include <config.h>
#include <ecs_systems.h>
#include <scene.h>

class Application {
public:
    static Application& application() {
        static Application application;
        return application;
    }

    static void init(int width, int height, const char* title) {
        application().init_glfw_private(width, height, title);
        application().init_entt_private();
        is_initialized = true;
    }

    static void run() {
        application().run_private();
    }

private:
    Application() {
        Logger::debug("Application ctor()");
    }

    ~Application() {
        Logger::debug("Application dtor()");
        if (window_ != nullptr) {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }
        registry_.clear();
    }

    inline static bool is_initialized = false;
    inline static GLFWwindow* window_ = nullptr;
    inline static std::unordered_map<int, bool> keyboard_key_map_;
    inline static std::unordered_map<int, bool> mouse_button_map_;
    inline static glm::vec2 mouse_last_coordinates_;

    inline static Scene scene_;
    inline static entt::registry registry_;
    inline static entt::entity root_entity_ = entt::null;

    void init_glfw_private(int width, int height, const char* title);
    void init_entt_private();
    void run_private();

    static void process_input(GLFWwindow* window);
    static void render(GLFWwindow* window);

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};
