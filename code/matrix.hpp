/*
  created by sy 2023/5/31
*/
#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <eigen3/Eigen/Eigen>
namespace matrix {
class mat{
public:
  static auto aTOr(float angle) -> float;
  static auto get_view_matrix(Eigen::Vector3f eye_pos) -> Eigen::Matrix4f;
  static auto get_model_matrix(float rotation_angle) -> Eigen::Matrix4f;
  static auto get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar) -> Eigen::Matrix4f;
  static auto get_projection_matrix_origin(float eye_fov, float aspect_ratio, float zNear, float zFar) -> Eigen::Matrix4f;
};
}
#endif