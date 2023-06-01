/*
  Created by sy on 2023/5/29
*/

#ifndef RASTERUZAER_HPP
#define RASTERUZAER_HPP

#include "Texture.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include <eigen3/Eigen/Eigen>
#include <vector>
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

struct nor_buf_id {
  int nor_id = 0;
};

class rasterizer {
private:
  int width, height; // screen size
  // MVP Matrix
  Eigen::Matrix4f model;
  Eigen::Matrix4f view;
  Eigen::Matrix4f projection;
  //
  int normal_id = -1;
  // mapping
  std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
  std::map<int, std::vector<Eigen::Vector3i>> ind_buf;
  std::map<int, std::vector<Eigen::Vector3f>> col_buf;
  std::map<int, std::vector<Eigen::Vector3f>> nor_buf;
  // texture
  std::optional<Texture> texture;
  //
  std::function<Eigen::Vector3f(fragment_shader_payload)> fragment_shader;
  std::function<Eigen::Vector3f(vertex_shader_payload)> vertex_shader;
  // frame buffer
  std::vector<Eigen::Vector3f> frame_buf;
  // depth buffer
  std::vector<float> depth_buf;
  // next id(point to the )
  int next_id = 0;
  // SSAA
  static const int N = 2;
  std::vector<std::array<float, N * N>> depth_buf_SSAA;
  std::vector<std::array<Eigen::Vector3f, N * N>> color_buf_SSAA;

public:
  // init rasterizer(width, height)
  rasterizer(int w, int h);
  // add id information
  auto load_positions(const std::vector<Eigen::Vector3f> &positions)
      -> pos_buf_id;
  auto load_indices(const std::vector<Eigen::Vector3i> &indices) -> ind_buf_id;
  auto load_colors(const std::vector<Eigen::Vector3f> &colors) -> col_buf_id;
  auto load_normals(const std::vector<Eigen::Vector3f> &normals) -> nor_buf_id;
  // MVP transform use homogeneous coordinates
  void set_model(const Eigen::Matrix4f &m);
  void set_view(const Eigen::Matrix4f &v);
  void set_projection(const Eigen::Matrix4f &p);
  // set texture
  inline void set_texture(Texture tex) { texture = tex; }
  // set shader
  void set_vertex_shader(std::function<Eigen::Vector3f(vertex_shader_payload)> vert_shader);
  void set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader);
  // set a pixel
  void set_pixel(const Eigen::Vector3f &point, const Eigen::Vector3f &color);
  // clear buffer
  void clear(Buffers buff);
  // draw
  void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer,
            Primitive type);
  void draw(std::vector<Triangle *> &TriangleList);

  auto frame_buffer() -> std::vector<Eigen::Vector3f> &;

private:
  void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);
  // use class triangle
  void rasterize_triangle(const Triangle &t);
  void rasterize_triangle(const Triangle &t, const std::array<Eigen::Vector3f, 3> &world_pos);
  void rasterize_triangle_SSAA(const Triangle &t, const int N);
  auto get_index(int x, int y) -> int;
  auto get_next_id() -> int;
};

} // namespace rst

#endif