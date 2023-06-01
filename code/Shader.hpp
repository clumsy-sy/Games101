/*
  Created by sy on 2023/6/1
*/

#ifndef SHADER_HPP
#define SHADER_HPP
#include "Texture.hpp"
#include <eigen3/Eigen/Eigen>
#include <utility>

struct fragment_shader_payload {
  Eigen::Vector3f view_pos, color, normal;
  Eigen::Vector2f tex_coords;
  Texture *texture;

  fragment_shader_payload() { texture = nullptr; }

  fragment_shader_payload(Eigen::Vector3f col, Eigen::Vector3f nor,
                          Eigen::Vector2f tc, Texture *tex)
      : color(std::move(col)), normal(std::move(nor)), tex_coords(std::move(tc)), texture(tex) {}
};

struct vertex_shader_payload {
  Eigen::Vector3f position;
};

#endif // RASTERIZER_SHADER_H
