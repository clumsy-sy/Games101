#ifndef RAY_COLOR_HPP
#define RAY_COLOR_HPP

#include "material.hpp"
#include "ray.hpp"
#include "hittable.hpp"

auto ray_color(const ray &r, const hittable &world, int depth) -> color {
  if (depth <= 0)
    return {0, 0, 0};
  hit_record rec;
  if (world.hit(r, 0.001, infinity, rec)) {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return {0, 0, 0};
  }
  Vec3d unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0); // 天空颜色的插值
}

auto ray_color(const ray &r, const color &background, const hittable &world, int depth) -> color {
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0)
    return {0, 0, 0};

  // If the ray hits nothing, return the background color.
  if (!world.hit(r, 0.001, infinity, rec))
    return background;

  ray scattered;
  color attenuation;
  color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}
#endif