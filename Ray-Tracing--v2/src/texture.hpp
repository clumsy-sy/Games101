#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <utility>

#include "global.hpp"
#include "Vec3dx4.hpp"
#include "perlin.hpp"

class texture {
public:
  [[nodiscard]] virtual auto value(double u, double v, const point3 &p) const -> color = 0;
};

class solid_color : public texture {
private:
  color color_value;

public:
  solid_color() = default;
  solid_color(color c) : color_value(std::move(c)) {}
  solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

  [[nodiscard]] auto value(double u, double v, const Vec3d &p) const -> color override {
    return color_value;
  }
};
// 棋盘格纹理
class checker_texture : public texture {
public:
  std::shared_ptr<texture> odd;
  std::shared_ptr<texture> even;

public:
  checker_texture() = default;

  checker_texture(std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd)
      : odd(std::move(_odd)), even(std::move(_even)) {}

  checker_texture(color c1, color c2)
      : odd(std::make_shared<solid_color>(c2)), even(std::make_shared<solid_color>(c1)) {}

  auto value(double u, double v, const point3 &p) const -> color override {
    auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
    if (sines < 0)
      return odd->value(u, v, p);
    else
      return even->value(u, v, p);
  }
};

class noise_texture : public texture {
public:
  perlin noise;
  double scale;

public:
  noise_texture() = default;
  noise_texture(double sc) : scale(sc) {}

  [[nodiscard]] auto value(double u, double v, const point3 &p) const -> color override {
    return color(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
  }
};

#endif