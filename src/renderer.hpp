#pragma once

#include "math.hpp"
#include "objects.hpp"

struct PixelRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

inline PixelRGB vec3_to_pixel(vec3 p) {
    return {
        .r = double_to_byte_color(p.x()),
        .g = double_to_byte_color(p.y()),
        .b = double_to_byte_color(p.z()),
    };
}

struct ImageRGB {
    size_t w;
    size_t h;
    PixelRGB* data;

    ImageRGB(size_t _w, size_t _h) : w(_w), h(_h) {
        data = new PixelRGB[w * h];
    }

    void set_pixel(size_t r, size_t c, PixelRGB pixel) {
        data[w * r + c] = pixel;
    }

    PixelRGB get_pixel(size_t r, size_t c) const {
        return data[w * r + c];
    }

    ~ImageRGB() {
        delete data;
    }
};

struct Renderer {
    double focal_length = 1.0;

    // 0 will get it from std::thread::hardware_concurrency
    unsigned int threads = 1; 
    
    // TODO better
    double viewport_height = 2.0;

    vec3 camera_center = vec3(0, 0, 0);
    int samples_per_pixel = 4;

    void render(const World& world, ImageRGB& image);
};
