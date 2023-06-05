/*
    Created by sy on 2023/5/29
*/

#include "Triangle.hpp"
#include <iostream>
#include <utility>

Triangle::Triangle() {
  v[0] << 0, 0, 0, 1;
  v[1] << 0, 0, 0, 1;
  v[2] << 0, 0, 0, 1;

  color[0] << 0.0, 0.0, 0.0;
  color[1] << 0.0, 0.0, 0.0;
  color[2] << 0.0, 0.0, 0.0;

  tex_coords[0] << 0.0, 0.0;
  tex_coords[1] << 0.0, 0.0;
  tex_coords[2] << 0.0, 0.0;
}

void Triangle::setVertex(int ind, Vector4f ver) { v[ind] = std::move(ver); }

void Triangle::setNormal(int ind, Vector3f n) { normal[ind] = std::move(n); }

void Triangle::setColor(int ind, float r, float g, float b) {
  if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) ||
      (b > 255.)) {
    std::cerr << "ERROR! Invalid color values" << std::endl;
    std::terminate();
  }

  color[ind] = Vector3f((float)r / 255., (float)g / 255., (float)b / 255.);
  return;
}
void Triangle::setNormals(const std::array<Vector3f, 3> &normals) {
  normal[0] = normals[0];
  normal[1] = normals[1];
  normal[2] = normals[2];
}
void Triangle::setColors(const std::array<Vector3f, 3> &colors) {
  setColor(0, colors[0][0], colors[0][1], colors[0][2]);
  setColor(1, colors[1][0], colors[1][1], colors[1][2]);
  setColor(2, colors[2][0], colors[2][1], colors[2][2]);
}

void Triangle::setTexCoord(int ind, float s, float t) {
  tex_coords[ind] = Vector2f(s, t);
}
void Triangle::setTexCoord(int ind, Eigen::Vector2f uv) {
  tex_coords[ind] = std::move(uv);
}

auto Triangle::toVector4() const -> std::array<Vector4f, 3> {
  std::array<Eigen::Vector4f, 3> res;
  std::transform(std::begin(v), std::end(v), res.begin(), [](auto &vec) {
    return Eigen::Vector4f(vec.x(), vec.y(), vec.z(), 1.f);
  });
  return res;
}
