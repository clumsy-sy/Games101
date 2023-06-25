#ifndef XY_RECT_HPP
#define XY_RECT_HPP

#include <utility>

#include "global.hpp"
#include "hittable.hpp"
// 在 xy 屏幕上的矩形，z 固定
class xy_rect : public hittable {
public:
  std::shared_ptr<material> mp;
  double x0, x1, y0, y1, k;

public:
  xy_rect() = default;

  xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, std::shared_ptr<material> mat)
      : mp(std::move(mat)), x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k){};

  auto hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool override;

  auto bounding_box(double, double, aabb &output_box) const -> bool override {
    // The bounding box must have non-zero width in each dimension, so pad the Z
    // dimension a small amount.
    output_box = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
    return true;
  }
};

auto xy_rect::hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool {
  auto t = (k - r.origin().z()) / r.direction().z();
  if (t < t_min || t > t_max)
    return false;
  auto x = r.origin().x() + t * r.direction().x();
  auto y = r.origin().y() + t * r.direction().y();
  if (x < x0 || x > x1 || y < y0 || y > y1)
    return false;
  rec.u = (x - x0) / (x1 - x0);
  rec.v = (y - y0) / (y1 - y0);
  rec.t = t;
  auto outward_normal = Vec3d(0, 0, 1);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.at(t);
  return true;
}

#endif