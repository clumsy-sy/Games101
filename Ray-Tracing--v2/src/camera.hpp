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
  Vec3d u, v, w;       // w:看向方向，u：镜头平面的 x，v：镜头屏幕的 y
  double lens_radius;  // 镜头半径
  double time0, time1; // shutter open/close times

public:
  camera() = default;
  camera(const point3 &lookfrom, const point3 &lookat, const Vec3d &vup, double vfov, double aspect_ratio,
      double aperture, double focus_dist, double t0, double t1) {
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
    time0 = t0;
    time1 = t1;
  }
  [[nodiscard]] auto get_ray(double s, double t) const -> ray {
    Vec3d rd = lens_radius * random_in_unit_disk(); // 镜头中随机一个点
    Vec3d offset = u * rd.x() + v * rd.y();

    return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset,
        random_double(time0, time1)());
  }
};
#endif