#include <config.h>
#include <iostream>

#include <application.h>

int main() {
    Application::init(800, 800, "Engine");
    Application::run();
    return 0;
}
