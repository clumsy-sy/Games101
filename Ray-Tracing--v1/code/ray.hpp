#ifndef RAY_HPP
#define RAY_HPP

#include "Vec3d.hpp"
class ray {
public:
  point3 orig;
  Vec3d dir;

public:
  ray() = default;
  ray(const point3 &origin, const Vec3d &direction)
      : orig(origin), dir(direction) {}

  [[nodiscard]] auto origin() const -> point3 { return orig; }
  [[nodiscard]] auto direction() const -> Vec3d { return dir; }
  [[nodiscard]] auto at(double t) const -> point3 { return orig + t * dir; }
};

#endif