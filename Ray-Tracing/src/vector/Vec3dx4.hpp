#ifndef VECTOR3DX4_HPP
#define VECTOR3DX4_HPP
#include "../global.hpp"
#include "../external/simd.hpp"

#include <cmath>
#include <iostream>
/*
  向量
*/
class Vec3d {
public:
  using f64x4 = simd<double, 4>;
  f64x4 e;

public:
  Vec3d() : e() {}
  Vec3d(const Vec3d &) = default;
  Vec3d(Vec3d &&) = default;
  auto operator=(const Vec3d &v) -> Vec3d & {
    e.r = v.e.r;
    return *this;
  };
  auto operator=(Vec3d &&v) noexcept -> Vec3d & {
    e.r = v.e.r;
    return *this;
  };
  Vec3d(double xx) : e(xx, xx, xx, 0) {}
  Vec3d(double xx, double yy, double zz) : e(xx, yy, zz, 0) {}
  Vec3d(const f64x4 &other) : e(other.r) {}

  auto operator-() const -> Vec3d {
    return -e;
  }
  auto operator[](int i) const -> double {
    return i < 3 ? e[i] : 0;
  }
  auto operator[](int i) -> double & {
    return i < 3 ? e[i] : e[3];
  }
  [[nodiscard]] auto x() const -> double {
    return e[0];
  }
  [[nodiscard]] auto y() const -> double {
    return e[1];
  }
  [[nodiscard]] auto z() const -> double {
    return e[2];
  }
  [[nodiscard]] auto sum3() const -> double {
    return x() + y() + z();
  }
  auto operator*(const double &r) const -> Vec3d {
    return e * r;
  }
  auto operator/(const double &r) const -> Vec3d {
    return e / r;
  }
  auto operator*(const Vec3d &v) const -> Vec3d {
    return e * v.e;
  }
  auto operator-(const Vec3d &v) const -> Vec3d {
    return e - v.e;
  }
  auto operator+(const Vec3d &v) const -> Vec3d {
    return e + v.e;
  }
  auto operator+=(const Vec3d &v) -> Vec3d & {
    e += v.e;
    return *this;
  }
  auto operator*=(const double &r) -> Vec3d & {
    e *= r;
    return *this;
  }
  auto operator/=(const double &r) -> Vec3d & {
    return *this *= 1 / r;
  }
  friend auto operator*(const double &r, const Vec3d &v) -> Vec3d {
    return v * r;
  }
  friend auto operator<<(std::ostream &os, const Vec3d &v) -> std::ostream & {
    return os << v.x() << ", " << v.y() << ", " << v.z();
  }
  [[nodiscard]] auto length_squared() const -> double {
    return Vec3d(e * e).sum3();
  }
  [[nodiscard]] auto length() const -> double {
    return std::sqrt(length_squared());
  }
  inline static auto random() -> Vec3d {
    return {random_double(), random_double(), random_double()};
  }

  inline static auto random(double min, double max) -> Vec3d {
    auto fun = random_double(min, max);
    return {fun(), fun(), fun()};
  }
  [[nodiscard]] auto near_zero() const -> bool {
    // 是否为 0 向量
    return (std::fabs(x()) < esp) && (std::fabs(y()) < esp) && (std::fabs(z()) < esp);
    // return length_squared() < esp3;
  }
  auto sqrt() -> Vec3d & {
    auto res = std::sqrt(e);
    this->e = res.r;
    return *this;
  }
};

// inline auto sqrt(Vec3d &v) -> void {
//   v = Vec3d(std::sqrt(v.e));
// }

inline auto dot(const Vec3d &a, const Vec3d &b) -> double {
  return (a * b).sum3();
}

inline auto cross(const Vec3d &a, const Vec3d &b) -> Vec3d {
  return {a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x()};
}
inline auto unit_vector(Vec3d v) -> Vec3d {
  return v /= v.length();
}

inline auto reflect(const Vec3d &v, const Vec3d &n) -> Vec3d {
  // 求镜面反射光线方向向量
  return v - 2 * dot(v, n) * n;
}

inline auto refract(const Vec3d &uv, const Vec3d &n, double etai_over_etat) -> Vec3d {
  // 聂耳定律
  auto cos_theta = std::min(dot(-uv, n), 1.0);
  Vec3d r_out_perp = etai_over_etat * (uv + cos_theta * n);
  Vec3d r_out_parallel = -std::sqrt(std::abs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}

inline auto random_in_unit_sphere() -> Vec3d {
  // 随机生成一个在单位圆内的点
  while (true) {
    auto p = Vec3d::random(-1, 1);
    if (p.length_squared() >= 1)
      continue;
    return p;
  }
}
inline auto random_unit_vector() -> Vec3d {
  // 生成一个随机方向的单位向量
  return unit_vector(random_in_unit_sphere());
}

inline auto random_in_hemisphere(const Vec3d &normal) -> Vec3d {
  // 判断与法线的位置关系
  Vec3d in_unit_sphere = random_in_unit_sphere();
  if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
    return in_unit_sphere;
  else
    return -in_unit_sphere;
}

auto random_in_unit_disk() -> Vec3d {
  // 平面单位圆内随机一点（z = 0）；
  auto fun = random_double(-1, 1);
  while (true) {
    auto p = Vec3d(fun(), fun(), 0);
    if (p.length_squared() >= 1)
      continue;
    return p;
  }
}

inline auto lerp(const Vec3d &a, const Vec3d &b, const float &t) -> Vec3d {
  // 线性插值
  return a * (1 - t) + b * t;
}
// point3 and color can use vector to representation
using point3 = Vec3d;
using color = Vec3d;

#endif