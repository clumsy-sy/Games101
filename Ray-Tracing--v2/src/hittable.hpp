#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#include <utility>

#include "Vec3dx4.hpp"
#include "ray.hpp"
#include "AABB.hpp"

class material;

struct hit_record {
  point3 p;
  Vec3d normal;
  std::shared_ptr<material> mat_ptr;
  double t;
  double u, v;
  bool front_face;

  inline void set_face_normal(const ray &r, const Vec3d &outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable {
public:
  virtual auto hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool = 0;
  virtual auto bounding_box(double time0, double time1, aabb &output_box) const -> bool = 0;
};

class translate : public hittable {
public:
  std::shared_ptr<hittable> ptr;
  Vec3d offset;

public:
  translate(std::shared_ptr<hittable> p, Vec3d displacement) : ptr(std::move(p)), offset(std::move(displacement)) {}

  auto hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool override;

  auto bounding_box(double time0, double time1, aabb &output_box) const -> bool override;
};

auto translate::hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool {
  ray moved_r(r.origin() - offset, r.direction(), r.time());
  if (!ptr->hit(moved_r, t_min, t_max, rec))
    return false;

  rec.p += offset;
  rec.set_face_normal(moved_r, rec.normal);

  return true;
}

auto translate::bounding_box(double time0, double time1, aabb &output_box) const -> bool {
  if (!ptr->bounding_box(time0, time1, output_box))
    return false;

  output_box = aabb(output_box.min() + offset, output_box.max() + offset);

  return true;
}

class rotate_y : public hittable {
public:
  std::shared_ptr<hittable> ptr;
  double sin_theta;
  double cos_theta;
  bool hasbox;
  aabb bbox;

public:
  rotate_y(std::shared_ptr<hittable> p, double angle);

  auto hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool override;

  auto bounding_box(double, double, aabb &output_box) const -> bool override {
    output_box = bbox;
    return hasbox;
  }
};

rotate_y::rotate_y(std::shared_ptr<hittable> p, double angle) : ptr(std::move(p)) {
  auto radians = degrees_to_radians(angle);
  sin_theta = sin(radians);
  cos_theta = cos(radians);
  hasbox = ptr->bounding_box(0, 1, bbox);

  point3 min(infinity, infinity, infinity);
  point3 max(-infinity, -infinity, -infinity);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
        auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
        auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

        auto newx = cos_theta * x + sin_theta * z;
        auto newz = -sin_theta * x + cos_theta * z;

        Vec3d tester(newx, y, newz);

        for (int c = 0; c < 3; c++) {
          min[c] = fmin(min[c], tester[c]);
          max[c] = fmax(max[c], tester[c]);
        }
      }
    }
  }

  bbox = aabb(min, max);
}
auto rotate_y::hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool {
  auto origin = r.origin();
  auto direction = r.direction();

  origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
  origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

  direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
  direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

  ray rotated_r(origin, direction, r.time());

  if (!ptr->hit(rotated_r, t_min, t_max, rec))
    return false;

  auto p = rec.p;
  auto normal = rec.normal;

  p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
  p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

  normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
  normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

  rec.p = p;
  rec.set_face_normal(rotated_r, normal);

  return true;
}

#endif