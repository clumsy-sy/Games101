#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <utility>

#include "hittable.hpp"
#include "ray.hpp"
#include "texture.hpp"
struct hit_record;

/*
  base class: attenuation(颜色衰减) 产生的 scattered（散射光）emit (发光，否则黑色)
*/
class material {
public:
  virtual auto scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const -> bool = 0;
  [[nodiscard]] virtual auto emitted(double, double, const point3 &) const -> color {
    return {0, 0, 0};
  }
};

class diffuse_light : public material {
public:
  std::shared_ptr<texture> emit;

public:
  diffuse_light(std::shared_ptr<texture> a) : emit(std::move(a)) {}
  diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}

  auto scatter(const ray &, const hit_record &, color &, ray &) const -> bool override {
    return false;
  }

  [[nodiscard]] auto emitted(double u, double v, const point3 &p) const -> color override {
    return emit->value(u, v, p);
  }
};

// 随机方向散射材料
class isotropic : public material {
public:
  std::shared_ptr<texture> albedo;

public:
  isotropic(color c) : albedo(std::make_shared<solid_color>(c)) {}
  isotropic(std::shared_ptr<texture> a) : albedo(std::move(a)) {}

  auto scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const -> bool override {
    scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }
};
#endif