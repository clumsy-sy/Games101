#ifndef LAMBERTIAN_HPP
#define LAMBERTIAN_HPP

#include "material.hpp"
#include "../geometry/hittable.hpp"

class lambertian : public material {
public:
  color albedo; // 以某种概率分布衰减，albedo / p

public:
  lambertian(const color &a) : albedo(a) {}

  auto scatter(const ray &, const hit_record &rec, color &attenuation, ray &scattered) const -> bool override {
    // 得到一个在交点的单位相切圆上的随机散射方向向量
    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }
};

#endif