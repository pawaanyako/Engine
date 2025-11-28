#pragma once

#define MODELS_PATH "resources\\models\\"
#define SHADERS_PATH "resources\\shaders\\"

#include <config.h>
#include <ecs_systems.h>
#include <scene.h>

class Application {
public:
    Application(int width, int height, const char* title);
    ~Application();

    const Scene& Application::get_scene() const {
        return scene_;
    }
    const entt::registry& get_registry() const {
        return registry_;
    }
    const EntityManagmentSystem& Application::get_entity_managment_system() const {
        return entity_managment_system_;
    }

    void run();

private:
    GLFWwindow* window_;
    Scene scene_;
    entt::registry registry_;
    EntityManagmentSystem entity_managment_system_;

    void static framebuffer_size_callback(GLFWwindow* window, GLsizei width, GLsizei height);
    void static process_input(GLFWwindow* window);
    void render(GLFWwindow* window) const;
};
