/*
  created by sy on 2023/5/30
*/

#include "matrix.hpp"
#include <eigen3/Eigen/Eigen>
constexpr double MY_PI = 3.1415926;

auto matrix::mat::aTOr(float angle) -> float { return angle / 180 * MY_PI; }

auto matrix::mat::get_view_matrix(Eigen::Vector3f eye_pos) -> Eigen::Matrix4f {
  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
  // clang-format off
  view << 1, 0, 0, -eye_pos[0],
          0, 1, 0, -eye_pos[1],
          0, 0, 1, -eye_pos[2],
          0, 0, 0, 1;
  // clang-format on
  return view;
}

auto matrix::mat::get_model_matrix(float rotation_angle) -> Eigen::Matrix4f {
  Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
  double rad = aTOr(rotation_angle);
  // clang-format off
    // model << cos(rad), -sin(rad), 0, 0,
    //          sin(rad), cos(rad), 0, 0,
    //          0, 0, 1, 0,
    //          0, 0, 0, 1;
  model << cos(rad), 0, sin(rad), 0,
           0, 1, 0, 0,
           -sin(rad), 0, cos(rad), 0,
           0, 0, 0, 1;
  // clang-format on
  
  return model;
}
// notice the eye_fov need divide 2.
auto matrix::mat::get_projection_matrix(float eye_fov, float aspect_ratio,
                                        float zNear, float zFar)
    -> Eigen::Matrix4f {
  Eigen::Matrix4f projection;
  float top = -std::tan(aTOr(eye_fov / 2.0f) * abs(zNear));
  float right = top * aspect_ratio;
  // clang-format off
    projection << zNear/right, 0, 0, 0,
                  0, zNear/top, 0, 0,
                  0, 0, (zNear+zFar)/(zNear-zFar), (2*zNear*zFar)/(zFar-zNear),
                  0, 0, 1, 0;
  // clang-format on
  return projection;
}
// this is the origin function of get_projection_matrix
auto matrix::mat::get_projection_matrix_origin(float eye_fov,
                                               float aspect_ratio, float zNear,
                                               float zFar) -> Eigen::Matrix4f {
  Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
  float n = -zNear, f = -zFar;
  float t = std::abs(n) * std::tan(aTOr(eye_fov) / 2), r = t * aspect_ratio;
  float b = -t, l = -r;

  Eigen::Matrix4f PerspToOrtho, translate, scale;
  // clang-format off
    PerspToOrtho << n, 0, 0, 0,
                    0, n, 0, 0,
                    0, 0, n + f, -n * f,
                    0, 0, 1, 0;
    translate << 1, 0, 0, -(r + l) / 2,
                 0, 1, 0, -(t + b) / 2,
                 0, 0, 1, -(n + f) / 2,
                 0, 0, 0, 1;
    scale << 2 / (r - l), 0, 0, 0,
             0, 2 / (t - b), 0, 0,
             0, 0, 2 / (n - f), 0,
             0, 0, 0, 1;
  // clang-format on
  projection = scale * translate * PerspToOrtho;
  return projection;
}