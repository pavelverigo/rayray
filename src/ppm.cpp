#include <iostream>
#include "math.hpp"
#include "objects.hpp"
#include "renderer.hpp"

void write_color(std::ostream &out, PixelRGB p) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(p.r) << ' '
        << static_cast<int>(p.g) << ' '
        << static_cast<int>(p.b) << '\n';
}

int main() {
    int width = 600;
    int height = 400;

    World world;

    world.spheres.emplace_back(vec3(0, 0, -1), 0.5);
    world.spheres.emplace_back(vec3(0, -100.5, -1), 100);

    ImageRGB image(width, height);
    Renderer renderer;
    renderer.render(world, image);

    // Render
    std::cout << "P3\n" << width << ' ' << height << "\n255\n";

    for (int j = 0; j < height; ++j) {
        std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
        for (int i = 0; i < width; ++i) {
            write_color(std::cout, image.get_pixel(j, i));
        }
    }

    std::clog << "\rDone.                      \n";
}