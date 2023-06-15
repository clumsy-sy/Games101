#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "global.hpp"
#include "ray.hpp"
#include "Vec3dx4.hpp"

class camera {
private:
  point3 origin = {0, 0, 0};
  point3 lower_left_corner;
  Vec3d horizontal;
  Vec3d vertical;
  Vec3d u, v, w;
  double lens_radius;

public:
  camera() = default;
  camera(
      point3 lookfrom, point3 lookat, Vec3d vup, double vfov, double aspect_ratio, double aperture, double focus_dist) {
    auto theta = degrees_to_radians(vfov);
    auto h = tan(theta / 2);
    auto viewport_height = 2.0 * h;
    auto viewport_width = aspect_ratio * viewport_height;

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    origin = lookfrom;
    horizontal = focus_dist * viewport_width * u;
    vertical = focus_dist * viewport_height * v;
    lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

    lens_radius = aperture / 2;
  }
  [[nodiscard]] auto get_ray(double s, double t) const -> ray {
    Vec3d rd = lens_radius * random_in_unit_disk();
    Vec3d offset = u * rd.x() + v * rd.y();

    return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
  }
};
#endif