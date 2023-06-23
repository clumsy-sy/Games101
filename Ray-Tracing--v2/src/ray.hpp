#ifndef RAY_HPP
#define RAY_HPP

#include <utility>
#include "Vec3dx4.hpp"

class ray {
public:
  point3 orig;
  Vec3d dir;
  double tm;

public:
  ray() = default;
  ray(point3 origin, Vec3d direction, double t = 0.0) : orig(std::move(origin)), dir(std::move(direction)), tm(t) {}

  [[nodiscard]] auto origin() const -> point3 {
    return orig;
  }
  [[nodiscard]] auto direction() const -> Vec3d {
    return dir;
  }
  [[nodiscard]] auto time() const -> double {
    return tm;
  }
  [[nodiscard]] auto at(double t) const -> point3 {
    return orig + t * dir;
  }
};

#endif