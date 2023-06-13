#include "BMP.hpp"
#include "global.hpp"
#include "ray.hpp"

auto hit_sphere(const point3 &center, double radius, const ray &r) -> bool {
  Vec3d oc = r.origin() - center;
  auto a = dot(r.direction(), r.direction());
  auto b = 2.0 * dot(oc, r.direction());
  auto c = dot(oc, oc) - radius * radius;
  auto discriminant = b * b - 4 * a * c;
  return (discriminant > 0);
}

auto ray_color(const ray &r) -> color {
  if (hit_sphere(point3(0, 0, -1), 0.5, r))
    return {1, 0, 0};
  Vec3d unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y + 1.0);
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

auto main() -> int {
  // Image
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  // Camera

  auto viewport_height = 2.0;
  auto viewport_width = aspect_ratio * viewport_height;
  auto focal_length = 1.0;

  auto origin = point3(0, 0, 0);
  auto horizontal = Vec3d(viewport_width, 0, 0);
  auto vertical = Vec3d(0, viewport_height, 0);
  auto lower_left_corner =
      origin - horizontal / 2 - vertical / 2 - Vec3d(0, 0, focal_length);
  // Render
  bmp::bitmap photo(image_width, image_height);
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      auto u = double(i) / (image_width - 1);
      auto v = double(j) / (image_height - 1);
      ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
      color pixel_color = ray_color(r);
      photo.set(i, j, pixel_color);
    }
    UpdateProgress(j, image_height - 1);
  }
  photo.generate("image3.bmp");

  return 0;
}