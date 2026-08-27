#pragma once

#include <chrono>
#include <thread>

class TimeManager {
public:
    TimeManager() = delete;

    static void set_target_fps(float target_fps) {
        if (target_fps <= 0.0f) {
            target_fps_ = 0.0f;
            frame_duration_ns_ = std::chrono::nanoseconds(0);
        } else {
            target_fps_ = target_fps;
            frame_duration_ns_ = std::chrono::nanoseconds(
                static_cast<long long>(1e9 / target_fps_));
        }
    }
    static float get_target_fps() { return target_fps_; }
    static float get_frame_duration() { return frame_duration_ns_.count() / 1e9f; }

    static float time_since_start() { return time_since_start_; }
    static float unscaled_delta_time() { return unscaled_delta_time_; }
    static float time_scale() { return time_scale_; }
    static void set_time_scale(float scale) { time_scale_ = scale; }
    static float delta_time() { return delta_time_; }

    static int fps() { return fps_; }

    static void update() {
        auto now = std::chrono::steady_clock::now();

        if (!initialized_) {
            start_time_ = now;
            last_frame_time_ = now;
            initialized_ = true;
        }

        float current = std::chrono::duration<float>(now - start_time_).count();

        unscaled_delta_time_ = current - time_since_start_;
        delta_time_ = unscaled_delta_time_ * time_scale_;

        time_since_start_ = current;

        frame_count_++;
        fps_timer_ += unscaled_delta_time_;

        if (fps_timer_ >= 1.0f) {
            fps_ = frame_count_;
            frame_count_ = 0;
            fps_timer_ = 0.0f;

            Logger::debug(fps_);
        }
    }

    static void fps_limit() {
        if (frame_duration_ns_.count() == 0) {
            last_frame_time_ = std::chrono::steady_clock::now();
            return;
        }

        auto target_time = last_frame_time_ + frame_duration_ns_;

        auto sleep_margin = std::chrono::milliseconds(16);

        auto now = std::chrono::steady_clock::now();
        auto wake_time = target_time - sleep_margin;

        if (now < wake_time) {
            std::this_thread::sleep_until(wake_time);
        }

        while (std::chrono::steady_clock::now() < target_time) {
            std::this_thread::yield();
        }

        last_frame_time_ = std::chrono::steady_clock::now();
    }

private:
    static inline float target_fps_ = 0.0f;
    static inline std::chrono::nanoseconds frame_duration_ns_ = std::chrono::nanoseconds(0);

    static inline std::chrono::steady_clock::time_point start_time_;
    static inline std::chrono::steady_clock::time_point last_frame_time_;
    static inline bool initialized_ = false;

    static inline float time_since_start_ = 0.0f;
    static inline float unscaled_delta_time_ = 0.0f;
    static inline float time_scale_ = 1.0f;
    static inline float delta_time_ = 0.0f;

    static inline int frame_count_ = 0;
    static inline float fps_timer_ = 0.0f;
    static inline int fps_ = 0;
};
