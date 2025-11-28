#pragma once

#include <iostream>
#include <string>

enum LogLevel {
    LOG_NONE = 0,
    LOG_FATAL = 1 << 0,
    LOG_ERROR = 1 << 1,
    LOG_WARN = 1 << 2,
    LOG_INFO = 1 << 3,
    LOG_DEBUG = 1 << 4,
    LOG_ALL = LOG_FATAL | LOG_ERROR | LOG_WARN | LOG_INFO | LOG_DEBUG
};

#ifndef LEVEL
#define LEVEL LOG_ALL
#endif

class Logger {
public:
    template <typename... Args>
    static void debug(Args&&... args) {
        if (LEVEL & LOG_DEBUG) {
            std::cout << "[DEBUG] ";
            (std::cout << ... << args);
            std::cout << std::endl;
        }
    }

    template <typename... Args>
    static void info(Args&&... args) {
        if (LEVEL & LOG_INFO) {
            std::cout << "[INFO] ";
            (std::cout << ... << args);
            std::cout << std::endl;
        }
    }

    template <typename... Args>
    static void warn(Args&&... args) {
        if (LEVEL & LOG_WARN) {
            std::cout << "[WARN] ";
            (std::cout << ... << args);
            std::cout << std::endl;
        }
    }

    template <typename... Args>
    static void error(Args&&... args) {
        if (LEVEL & LOG_ERROR) {
            std::cout << "[ERROR] ";
            (std::cout << ... << args);
            std::cout << std::endl;
        }
    }

    template <typename... Args>
    static void fatal(Args&&... args) {
        if (LEVEL & LOG_FATAL) {
            std::cout << "[FATAL] ";
            (std::cout << ... << args);
            std::cout << std::endl;
        }
    }
};
