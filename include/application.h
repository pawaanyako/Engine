#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <config.h>
#include <scene.h>

class Application {
public:
    Application(int width, int height, const char* title);
    ~Application();

    void run();

private:
    GLFWwindow* window_ = nullptr;
    std::unique_ptr<Scene> scene_;
    std::unique_ptr<Shader> shader_;

    void static framebuffer_size_callback(GLFWwindow* window, GLsizei width, GLsizei height);
    void static process_input(GLFWwindow* window);
    void render(GLFWwindow* window) const;
};
#endif
