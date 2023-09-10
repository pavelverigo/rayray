#include "renderer.hpp"

static vec3 ray_color(const ray& r, const World& world) {
    HitData hit;
    if (world.hit(r, {0, infinity}, hit)) {
        return 0.5 * (hit.normal + vec3(1, 1, 1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.5, 0.7, 1.0);
}

void Renderer::render(const World& world, ImageRGB& image) {
    // Viewport widths less than one are ok since they are real valued.
    auto viewport_width = viewport_height * (static_cast<double>(image.w)/image.h);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image.w;
    auto pixel_delta_v = viewport_v / image.h;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    
    const auto pixel_sample_square = [&]() {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    };

    const auto get_ray = [&](int i, int j) {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }; 

    for (int j = 0; j < image.h; ++j) {
        for (int i = 0; i < image.w; ++i) {
            vec3 pixel_color(0, 0, 0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, world);
            }

            auto scale = 1.0 / samples_per_pixel;
            pixel_color *= scale;

            image.set_pixel(j, i, vec3_to_pixel(pixel_color));
        }
    }
}