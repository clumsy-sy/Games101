/*
  Created by sy on 2023/5/29
*/

#ifndef RASTERUZAER_HPP
#define RASTERUZAER_HPP

#include "Triangle.hpp"
#include <eigen3/Eigen/Eigen>
using namespace Eigen;

namespace rst {
enum class Buffers { Color = 1, Depth = 2 };

inline auto operator|(Buffers a, Buffers b) -> Buffers {
  return Buffers((int)a | (int)b);
}
inline auto operator&(Buffers a, Buffers b) -> Buffers {
  return Buffers((int)a & (int)b);
}

enum class Primitive { Line, Triangle };
/*
  Type safety
*/
struct pos_buf_id {
  int pos_id = 0;
};

struct ind_buf_id {
  int ind_id = 0;
};

struct col_buf_id {
  int col_id = 0;
};

class rasterizer {
private:
  int width, height; // screen size
  // MVP Matrix
  Eigen::Matrix4f model;
  Eigen::Matrix4f view;
  Eigen::Matrix4f projection;
  // mapping
  std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
  std::map<int, std::vector<Eigen::Vector3i>> ind_buf;
  std::map<int, std::vector<Eigen::Vector3f>> col_buf;
  // frame buffer
  std::vector<Eigen::Vector3f> frame_buf;
  // depth buffer
  std::vector<float> depth_buf;
  // next id(point to the )
  int next_id = 0;

public:
  // init rasterizer(width, height)
  rasterizer(int w, int h);
  // add id information
  auto load_positions(const std::vector<Eigen::Vector3f> &positions) -> pos_buf_id;
  auto load_indices(const std::vector<Eigen::Vector3i> &indices) -> ind_buf_id;
  auto load_colors(const std::vector<Eigen::Vector3f> &colors) -> col_buf_id;
  // MVP transform use homogeneous coordinates
  void set_model(const Eigen::Matrix4f &m);
  void set_view(const Eigen::Matrix4f &v);
  void set_projection(const Eigen::Matrix4f &p);
  // set a pixel
  void set_pixel(const Eigen::Vector3f &point, const Eigen::Vector3f &color);
  // clear buffer
  void clear(Buffers buff);
  // draw 
  void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);

  auto frame_buffer() -> std::vector<Eigen::Vector3f>&;

private:
  // use class triangle
  void rasterize_triangle(const Triangle &t);
  auto get_index(int x, int y) -> int;
  auto get_next_id() -> int;  
};

} // namespace rst

#endif