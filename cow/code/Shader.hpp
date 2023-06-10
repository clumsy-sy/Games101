/*
  Created by sy on 2023/6/1
*/

#ifndef SHADER_HPP
#define SHADER_HPP
#include "Texture.hpp"
#include <eigen3/Eigen/Eigen>
#include <utility>

class fragment_shader_payload {
public:
  Eigen::Vector3f view_pos, color, normal;
  Eigen::Vector2f tex_coords;
  Texture *texture;

  fragment_shader_payload() { texture = nullptr; }

  fragment_shader_payload(Eigen::Vector3f col, Eigen::Vector3f nor,
                          Eigen::Vector2f tc, Texture *tex)
      : color(std::move(col)), normal(std::move(nor)),
        tex_coords(std::move(tc)), texture(tex) {}
};

class vertex_shader_payload {
public:
  Eigen::Vector3f position;
};

struct light {
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

auto vertex_shader(const vertex_shader_payload &payload) -> Eigen::Vector3f;
auto normal_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f;
auto phong_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f;
auto texture_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f;
auto displacement_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f;
auto bump_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f;


#endif // RASTERIZER_SHADER_H
