/*
  Created by sy on 2023/6/5
*/

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Vector.hpp"
#include "global.hpp"

class Object {
public:
  // material properties
  MaterialType materialType = DIFFUSE_AND_GLOSSY;
  float ior = 1.3;
  float Kd = 0.8, Ks = 0.2;
  Vector3f diffuseColor = 0.2;
  float specularExponent = 25; // 镜面反射系数

public:
  Object()
      : diffuseColor(0.2) {}

  virtual ~Object() = default;

  virtual auto intersect(const Vector3f &, const Vector3f &, float &,
                         uint32_t &, Vector2f &) const -> bool = 0;

  virtual void getSurfaceProperties(const Vector3f &, const Vector3f &,
                                    const uint32_t &, const Vector2f &,
                                    Vector3f &, Vector2f &) const = 0;

  [[nodiscard]] virtual auto evalDiffuseColor(const Vector2f &) const -> Vector3f {
    return diffuseColor;
  }
};

#endif