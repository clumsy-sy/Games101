#ifndef SPHERE_H
#define SPHERE_H

#include <utility>

#include "Vec3dx4.hpp"
#include "hittable.hpp"

class sphere : public hittable {

public:
  point3 center;
  double radius, radius2;
  std::shared_ptr<material> mat_ptr;

public:
  sphere() = default;
  sphere(point3 c, double r, std::shared_ptr<material> m)
      : center(std::move(c)), radius(r), radius2(r * r), mat_ptr(std::move(m)){};

  auto hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool override;
  auto bounding_box(double, double, aabb &output_box) const -> bool override;

private:
  static void get_sphere_uv(const point3 &p, double &u, double &v) {
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
  }
};

auto sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const -> bool {
  Vec3d oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius2;
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
  Vec3d outward_normal = (rec.p - center) / radius;
  rec.set_face_normal(r, outward_normal);
  get_sphere_uv(outward_normal, rec.u, rec.v);
  rec.mat_ptr = mat_ptr;

  return true;
}

auto sphere::bounding_box(double, double, aabb &output_box) const -> bool {
  output_box = aabb(center - Vec3d(radius, radius, radius), center + Vec3d(radius, radius, radius));
  return true;
}

#endif