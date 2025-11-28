#include <config.h>
#include <iostream>

#include <application.h>

int main() {
    Application application(800, 800, "Engine");
    application.run();
    return 0;
}
