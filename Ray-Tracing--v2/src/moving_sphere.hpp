#ifndef MOVING_SPHERE_HPP
#define MOVING_SPHERE_HPP

#include <utility>
#include "hittable.hpp"

class moving_sphere : public hittable {
public:
  point3 center0, center1;
  double time0, time1;
  double radius;
  std::shared_ptr<material> mat_ptr;

public:
  moving_sphere() = default;
  moving_sphere(point3 cen0, point3 cen1, double _time0, double _time1, double r, std::shared_ptr<material> m)
      : center0(std::move(cen0)), center1(std::move(cen1)), time0(_time0), time1(_time1), radius(r),
        mat_ptr(std::move(m)){};

  auto hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool override {
    Vec3d oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    // Find the nearest root that lies in the acceptable range.
    double x0, x1, root;
    if (solveQuadratic_halfb(a, half_b, c, x0, x1)) {
      if (x0 > t_min && x0 < t_max)
        root = x0;
      else if (x1 > t_min && x1 < t_max)
        root = x1;
      else
        return false;
    } else
      return false;

    rec.t = root;
    rec.p = r.at(rec.t);
    auto outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
  }

  [[nodiscard]] auto center(double time) const -> point3 {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
  };
};

#endif