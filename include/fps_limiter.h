#pragma once

#include <chrono>
#include <thread>

class FPSLimiter {
public:
    FPSLimiter(double target_fps) {
        frameDuration_ = 1.0 / target_fps;
        lastFrameTime_ = std::chrono::steady_clock::now();
    }

    void limit() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastFrameTime_).count();

        double remaining = frameDuration_ - elapsed;
        if (remaining > 0.002) {
            std::this_thread::sleep_for(std::chrono::duration<double>(remaining - 0.001));
        }

        while (std::chrono::duration<double>(std::chrono::steady_clock::now() - lastFrameTime_).count() < frameDuration_) {
        }

        lastFrameTime_ = std::chrono::steady_clock::now();
    }

private:
    double frameDuration_;
    std::chrono::steady_clock::time_point lastFrameTime_;
};
