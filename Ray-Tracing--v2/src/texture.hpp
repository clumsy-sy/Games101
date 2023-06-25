#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <utility>

#include "global.hpp"
#include "Vec3dx4.hpp"
#include "perlin.hpp"
#include "rtw_stb_image.hpp"

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

  [[nodiscard]] auto value(double, double, const Vec3d &) const -> color override {
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

  [[nodiscard]] auto value(double u, double v, const point3 &p) const -> color override {
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

  [[nodiscard]] auto value(double, double, const point3 &p) const -> color override {
    // return color(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
    // return color(1, 1, 1) * noise.turb(scale * p);
    return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
  }
};

class image_texture : public texture {
private:
  unsigned char *data;
  int width, height;
  int bytes_per_scanline;

public:
  const static int bytes_per_pixel = 3;

  image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

  image_texture(const char *filename) {
    auto components_per_pixel = bytes_per_pixel;

    // data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
    std::cout << "width : " << width << " height: " << height << std::endl;
    if (!data) {
      std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
      width = height = 0;
    }

    bytes_per_scanline = bytes_per_pixel * width;
  }

  ~image_texture() {
    delete data;
  }

  [[nodiscard]] auto value(double u, double v, const Vec3d &) const -> color override {
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (data == nullptr)
      return {0, 1, 1};

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = clamp(u, 0.0, 1.0);
    v = 1.0 - clamp(v, 0.0, 1.0); // Flip V to image coordinates

    auto i = static_cast<int>(u * width);
    auto j = static_cast<int>(v * height);

    // Clamp integer mapping, since actual coordinates should be less than 1.0
    if (i >= width)
      i = width - 1;
    if (j >= height)
      j = height - 1;

    const auto color_scale = 1.0 / 255.0;
    auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

    return {color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]};
  }
};

#endif