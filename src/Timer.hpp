#pragma once

#include <iostream>
#include <chrono>

struct Timer {
    std::chrono::high_resolution_clock::time_point start_time;

    Timer() {
        reset();
    }

    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void printTimeElapsed() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        double seconds = static_cast<double>(duration) / 1e6;
        std::cout << "Timer: " << seconds << std::endl;
    }
};

