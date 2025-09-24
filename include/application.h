#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <config.h>

class Application {
public:
    Application(int width, int height, const char* title);
    ~Application();

    void run();

private:
    GLFWwindow* window_ = nullptr;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void process_input(GLFWwindow* window);
    static void render(GLFWwindow* window);
};
#endif
