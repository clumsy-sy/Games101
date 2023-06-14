#include "BMP.hpp"
#include "global.hpp"
#include "ray.hpp"

auto hit_sphere(const point3 &center, double radius, const ray &r) -> double {
  Vec3d oc = r.origin() - center;
  auto a = dot(r.direction(), r.direction());
  auto b = 2.0 * dot(oc, r.direction());
  auto c = dot(oc, oc) - radius * radius;
  auto discriminant = b * b - 4 * a * c;
  if (discriminant < 0) {
    return -1.0;
  } else {
    // 球在摄像机前方，第一次照射到的必然是 t 小的那个
    return (-b - sqrt(discriminant)) / (2.0 * a);
  }
}

auto hit_sphere_halfb(const point3 &center, double radius, const ray &r)
    -> double {
  Vec3d oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;
  // auto discriminant = half_b * half_b - a * c;
  double x0, x1;
  if (solveQuadratic_halfb(a, half_b, c, x0, x1)) {
    return x1;
  } else {
    return -1.0;
  }
}

auto ray_color(const ray &r) -> color {
  auto t = hit_sphere_halfb(point3(0, 0, -1), 0.5, r);
  // 光线照射到小球
  if (t > 0.0) {
    Vec3d N = unit_vector(r.at(t) - Vec3d(0, 0, -1));
    return 0.5 * color(N.x + 1, N.y + 1, N.z + 1);
  }
  // 背景
  Vec3d unit_direction = unit_vector(r.direction());
  t = 0.5 * (unit_direction.y + 1.0);
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
  photo.generate("image4.bmp");

  return 0;
}