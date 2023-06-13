/*
  by sy 2023/6/5
*/
#ifndef VECTOR_HPP
#define VECTOR_HPP
#include "global.hpp"
#include <cmath>
#include <iostream>
/*
  向量
*/
class Vec3d {
public:
  double x, y, z;

public:
  Vec3d() : x(0), y(0), z(0) {}
  Vec3d(double xx) : x(xx), y(xx), z(xx) {}
  Vec3d(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}

  auto operator-() const -> Vec3d { return {-x, -y, -z}; }
  auto operator[](int i) const -> double {
    if (i == 0)
      return x;
    else if (i == 1)
      return y;
    else if (i == 2)
      return z;
    else
      return 0.0;
  }
  auto operator*(const double &r) const -> Vec3d {
    return {x * r, y * r, z * r};
  }
  auto operator/(const double &r) const -> Vec3d {
    return {x / r, y / r, z / r};
  }
  auto operator*(const Vec3d &v) const -> Vec3d {
    return {x * v.x, y * v.y, z * v.z};
  }
  auto operator-(const Vec3d &v) const -> Vec3d {
    return {x - v.x, y - v.y, z - v.z};
  }
  auto operator+(const Vec3d &v) const -> Vec3d {
    return {x + v.x, y + v.y, z + v.z};
  }
  auto operator+=(const Vec3d &v) -> Vec3d & {
    x += v.x, y += v.y, z += v.z;
    return *this;
  }
  auto operator*=(const double &r) -> Vec3d & {
    x *= r, y *= r, z *= r;
    return *this;
  }
  auto operator/=(const double &r) -> Vec3d & { return *this *= 1 / r; }
  friend auto operator*(const double &r, const Vec3d &v) -> Vec3d {
    return {v.x * r, v.y * r, v.z * r};
  }
  friend auto operator<<(std::ostream &os, const Vec3d &v) -> std::ostream & {
    return os << v.x << ", " << v.y << ", " << v.z;
  }
  [[nodiscard]] auto length_squared() const -> double {
    return x * x + y * y + z * z;
  }
  [[nodiscard]] auto length() const -> double {
    return std::sqrt(length_squared());
  }
  inline static auto random() -> Vec3d {
    return {random_double(), random_double(), random_double()};
  }

  inline static auto random(double min, double max) -> Vec3d {
    return {random_double(min, max), random_double(min, max),
            random_double(min, max)};
  }
  [[nodiscard]] auto near_zero() const -> bool {
    // Return true if the vector is close to zero in all dimensions.
    const auto s = 1e-8;
    return (std::fabs(x) < s) && (std::fabs(y) < s) && (std::fabs(z) < s);
  }
};

inline auto sqrt(Vec3d &v) -> void {
  v.x = std::sqrt(v.x), v.y = std::sqrt(v.y), v.z = std::sqrt(v.z);
}

inline auto dot(const Vec3d &a, const Vec3d &b) -> double {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline auto cross(const Vec3d &a, const Vec3d &b) -> Vec3d {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
inline auto unit_vector(Vec3d v) -> Vec3d { return v / v.length(); }
// 求镜面反射光线方向向量
inline auto reflect(const Vec3d &v, const Vec3d &n) -> Vec3d {
  return v - 2 * dot(v, n) * n;
}
// 聂耳定律
inline auto refract(const Vec3d &uv, const Vec3d &n, double etai_over_etat)
    -> Vec3d {
  auto cos_theta = std::fmin(dot(-uv, n), 1.0);
  Vec3d r_out_perp = etai_over_etat * (uv + cos_theta * n);
  Vec3d r_out_parallel =
      -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}

/*
  随机生成一个在单位圆内的点
*/
inline auto random_in_unit_sphere() -> Vec3d {
  while (true) {
    auto p = Vec3d::random(-1, 1);
    if (p.length_squared() >= 1)
      continue;
    return p;
  }
}
inline auto random_unit_vector() -> Vec3d {
  return unit_vector(random_in_unit_sphere());
}

inline auto random_in_hemisphere(const Vec3d &normal) -> Vec3d {
  Vec3d in_unit_sphere = random_in_unit_sphere();
  if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
    return in_unit_sphere;
  else
    return -in_unit_sphere;
}

auto random_in_unit_disk() -> Vec3d {
  while (true) {
    auto p = Vec3d(random_double(-1, 1), random_double(-1, 1), 0);
    if (p.length_squared() >= 1)
      continue;
    return p;
  }
}

inline auto lerp(const Vec3d &a, const Vec3d &b, const float &t) -> Vec3d {
  return a * (1 - t) + b * t;
}
// point3 and color can use vector to representation
using point3 = Vec3d;
using color = Vec3d;

#endif