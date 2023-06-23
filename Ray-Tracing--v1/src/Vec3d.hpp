/*
  by sy 2023/6/5
*/
// #ifndef VECTOR_HPP
// #define VECTOR_HPP
// #include "global.hpp"
// #include <cmath>
// #include <iostream>
// #include <array>
/*
  向量
*/

// class vec3d {
// private:
//   std::array<double, 3> e;

// public:
//   vec3d() : e{0, 0, 0} {}
//   vec3d(double xx) : e{xx, xx, xx} {}
//   vec3d(double xx, double yy, double zz) : e{xx, yy, zz} {}

//   auto operator-() const -> vec3d {
//     return {-x(), -y(), -z()};
//   }
//   [[nodiscard]] auto x() const -> double {
//     return e[0];
//   }
//   [[nodiscard]] auto y() const -> double {
//     return e[1];
//   }
//   [[nodiscard]] auto z() const -> double {
//     return e[2];
//   }
//   [[nodiscard]] auto x() -> double & {
//     return e[0];
//   }
//   [[nodiscard]] auto y() -> double & {
//     return e[1];
//   }
//   [[nodiscard]] auto z() -> double & {
//     return e[2];
//   }
//   [[nodiscard]] auto sum3() const -> double {
//     return x() + y() + z();
//   }
//   auto operator[](int i) const -> double {
//     return e[i];
//   }
//   auto operator*(const double &r) const -> vec3d {
//     return {x() * r, y() * r, z() * r};
//   }
//   auto operator/(const double &r) const -> vec3d {
//     return {x() / r, y() / r, z() / r};
//   }
//   auto operator*(const vec3d &v) const -> vec3d {
//     return {x() * v.x(), y() * v.y(), z() * v.z()};
//   }
//   auto operator-(const vec3d &v) const -> vec3d {
//     return {x() - v.x(), y() - v.y(), z() - v.z()};
//   }
//   auto operator+(const vec3d &v) const -> vec3d {
//     return {x() + v.x(), y() + v.y(), z() + v.z()};
//   }
//   auto operator+=(const vec3d &v) -> vec3d & {
//     x() += v.x(), y() += v.y(), z() += v.z();
//     return *this;
//   }
//   auto operator*=(const double &r) -> vec3d & {
//     x() *= r, y() *= r, z() *= r;
//     return *this;
//   }
//   auto operator/=(const double &r) -> vec3d & {
//     return *this *= 1 / r;
//   }
//   friend auto operator*(const double &r, const vec3d &v) -> vec3d {
//     return {v.x() * r, v.y() * r, v.z() * r};
//   }
//   friend auto operator<<(std::ostream &os, const vec3d &v) -> std::ostream & {
//     return os << v.x() << ", " << v.y() << ", " << v.z();
//   }
//   [[nodiscard]] auto length_squared() const -> double {
//     return x() * x() + y() * y() + z() * z();
//   }
//   [[nodiscard]] auto length() const -> double {
//     return std::sqrt(length_squared());
//   }
//   inline static auto random() -> vec3d {
//     return {random_double(), random_double(), random_double()};
//   }

//   inline static auto random(double min, double max) -> vec3d {
//     auto fun = random_double(min, max);
//     return {fun(), fun(), fun()};
//   }
//   [[nodiscard]] auto near_zero() const -> bool {
//     // Return true if the vector is close to zero in all dimensions.
//     return (std::fabs(x()) < esp) && (std::fabs(y()) < esp) && (std::fabs(z()) < esp);
//   }
// };

// inline auto sqrt(vec3d &v) -> void {
//   v.x() = std::sqrt(v.x()), v.y() = std::sqrt(v.y()), v.z() = std::sqrt(v.z());
// }

// inline auto dot(const vec3d &a, const vec3d &b) -> double {
//   return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
// }

// inline auto cross(const vec3d &a, const vec3d &b) -> vec3d {
//   return {a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x()};
// }
// inline auto unit_vector(vec3d v) -> vec3d {
//   return v / v.length();
// }
// // 求镜面反射光线方向向量
// inline auto reflect(const vec3d &v, const vec3d &n) -> vec3d {
//   return v - 2 * dot(v, n) * n;
// }
// // 聂耳定律
// inline auto refract(const vec3d &uv, const vec3d &n, double etai_over_etat) -> vec3d {
//   auto cos_theta = std::fmin(dot(-uv, n), 1.0);
//   vec3d r_out_perp = etai_over_etat * (uv + cos_theta * n);
//   vec3d r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
//   return r_out_perp + r_out_parallel;
// }

// /*
//   随机生成一个在单位圆内的点
// */
// inline auto random_in_unit_sphere() -> vec3d {
//   while (true) {
//     auto p = vec3d::random(-1, 1);
//     if (p.length_squared() >= 1)
//       continue;
//     return p;
//   }
// }
// inline auto random_unit_vector() -> vec3d {
//   return unit_vector(random_in_unit_sphere());
// }

// inline auto random_in_hemisphere(const vec3d &normal) -> vec3d {
//   vec3d in_unit_sphere = random_in_unit_sphere();
//   if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
//     return in_unit_sphere;
//   else
//     return -in_unit_sphere;
// }

// auto random_in_unit_disk() -> vec3d {
//   auto fun = random_double(-1, 1);
//   while (true) {
//     auto p = vec3d(fun(), fun(), 0);
//     if (p.length_squared() >= 1)
//       continue;
//     return p;
//   }
// }

// inline auto lerp(const vec3d &a, const vec3d &b, const float &t) -> vec3d {
//   return a * (1 - t) + b * t;
// }
// point3 and color can use vector to representation
// using point3 = vec3d;
// using color = vec3d;

// #endif