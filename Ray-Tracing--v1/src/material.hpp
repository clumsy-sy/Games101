#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "ray.hpp"
struct hit_record;

/*
  base class: attenuation(颜色衰减) 产生的 scattered（散射光）
*/

class material {
public:
  virtual auto scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const -> bool = 0;
};

#endif