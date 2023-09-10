#pragma once

#include "math.hpp"
#include <vector>

struct HitData {
    vec3 p;
    vec3 normal;
    double t;
};

struct Sphere {
    vec3 center;
    double radius;

    Sphere(vec3 _center, double _radius) : center(_center), radius(_radius) {}

    bool hit(const ray& r, interval ray_t, HitData& hit) const {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        auto root = (-half_b - sqrtd) / a;
        if (root <= ray_t.min || ray_t.max <= root) {
            root = (-half_b + sqrtd) / a;
            if (root <= ray_t.min || ray_t.max <= root)
                return false;
        }

        hit.t = root;
        hit.p = r.at(hit.t);
        hit.normal = (hit.p - center) / radius;

        return true;
    }
};

struct World {
    std::vector<Sphere> spheres;

    bool hit(const ray& r, interval ray_t, HitData& rec) const {
        HitData temp_hit;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& sphere : spheres) {
            if (sphere.hit(r, {ray_t.min, closest_so_far}, temp_hit)) {
                hit_anything = true;
                closest_so_far = temp_hit.t;
                rec = temp_hit;
            }
        }

        return hit_anything;
    }
};