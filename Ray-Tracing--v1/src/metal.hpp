#ifndef METAL_HPP
#define METAL_HPP

#include "material.hpp"
#include "Vec3dx4.hpp"
#include "hittable.hpp"

class metal : public material {
public:
  color albedo;
  double fuzz; // 模糊参数 0 为不扰动

public:
  metal(const color &a) : albedo(a) {
    fuzz = 0;
  }
  metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  auto scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const -> bool override {
    Vec3d reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }
};

#endif