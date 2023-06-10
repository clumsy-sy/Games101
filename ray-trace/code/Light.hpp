/*
  Created by sy on 2023/6/9
*/

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Vector.hpp"

class Light {
public:
  Vector3f position;
  Vector3f intensity;

public:
  Light(const Vector3f &p, const Vector3f &i) : position(p), intensity(i) {}
  virtual ~Light() = default;
};

#endif