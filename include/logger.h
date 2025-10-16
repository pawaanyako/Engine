#ifndef LOGGER_H_
#define LOGGER_H_

#define DEBUG true

#include <iostream>
#include <string>

class Logger {
public:
    static void debug(const std::string& message) {
        if (DEBUG) {
            std::cout << "[DEBUG] " << message << std::endl;
        }
    }
};

#endif
