/*
  Created by sy 2023/6/7
*/

#include "Sphere.hpp"

auto Sphere::intersect(const Vector3f &orig, const Vector3f &dir, float &tnear,
                       uint32_t &, Vector2f &) const -> bool {
  // analytic solution
  Vector3f L = orig - center;
  float a = dotProduct(dir, dir);
  float b = 2 * dotProduct(dir, L);
  float c = dotProduct(L, L) - radius2;
  float t0, t1;
  if (!solveQuadratic(a, b, c, t0, t1))
    return false;
  if (t0 < 0) {
    t0 = t1;
    return false;
  }
  tnear = t0;
  return true;
}

void Sphere::getSurfaceProperties(const Vector3f &P, const Vector3f &,
                                  const uint32_t &, const Vector2f &,
                                  Vector3f &N, Vector2f &) const {
  N = normalize(P - center);
}