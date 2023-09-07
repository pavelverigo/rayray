#pragma once

#include "math.hpp"
#include <vector>

struct hit_record {
    vec3 p;
    vec3 normal;
    double t;
};

struct sphere {
    vec3 center;
    double radius;

    sphere(vec3 _center, double _radius) : center(_center), radius(_radius) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root <= ray_t.min || ray_t.max <= root) {
            root = (-half_b + sqrtd) / a;
            if (root <= ray_t.min || ray_t.max <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = (rec.p - center) / radius;

        return true;
    }
};

struct world {
    std::vector<sphere> spheres;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& sphere : spheres) {
            if (sphere.hit(r, {ray_t.min, closest_so_far}, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};