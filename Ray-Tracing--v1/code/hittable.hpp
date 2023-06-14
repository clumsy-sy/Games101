#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#include "Vec3d.hpp"
#include "ray.hpp"

struct hit_record {
  point3 p;
  Vec3d normal;
  double t;
  bool front_face;

  inline void set_face_normal(const ray &r, const Vec3d &outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable {
public:
  virtual auto hit(const ray &r, double t_min, double t_max,
                   hit_record &rec) const -> bool = 0;
};

#endif