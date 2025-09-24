#include <config.h>
#include <iostream>

#include <application.h>

int main() {
    try {
        // Application application(1280, 720, "Engine");
        Application application(800, 800, "Engine");
        application.run();
    } catch (const std::exception& e) {
        std::cerr << e.what();
    }
    return 0;
}
