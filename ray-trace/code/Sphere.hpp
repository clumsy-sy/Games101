#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Object.hpp"
#include "Vector.hpp"
/*
  球面（中心坐标，半径，半径平方）
*/
class Sphere : public Object {
public:
  Vector3f center;
  float radius, radius2;

public:
  Sphere(const Vector3f &c, const float &r)
      : center(c), radius(r), radius2(r * r) {}

  auto intersect(const Vector3f &orig, const Vector3f &dir, float &tnear,
                 uint32_t &, Vector2f &) const -> bool override;

  void getSurfaceProperties(const Vector3f &P, const Vector3f &,
                            const uint32_t &, const Vector2f &, Vector3f &N,
                            Vector2f &) const override;
};
#endif