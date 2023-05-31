/*
        Created by sy on 2023/5/29
*/

#include "rasterizer.hpp"

// vec3 to vec4
auto to_vec4(const Eigen::Vector3f &v3, float w = 1.0f) {
  return Vector4f(v3.x(), v3.y(), v3.z(), w);
}
// judge a point is inside the triangle?
static auto insideTriangle(int x, int y,
                           const std::array<Eigen::Vector3f, 3> &_v) -> bool {
  const Vector3f &A = _v[0];
  const Vector3f &B = _v[1];
  const Vector3f &C = _v[2];

  Vector3f AB = B - A;
  Vector3f BC = C - B;
  Vector3f CA = A - C;

  Vector3f P(x + 0.5f, y + 0.5f, 1.0f);

  Vector3f AP = P - A;
  Vector3f BP = P - B;
  Vector3f CP = P - C;

  float z1 = AB.cross(AP).z();
  float z2 = BC.cross(BP).z();
  float z3 = CA.cross(CP).z();

  return (z1 > 0 && z2 > 0 && z3 > 0) || (z1 < 0 && z2 < 0 && z3 < 0);
}
static auto computeBarycentric2D(float x, float y,
                                 const std::array<Eigen::Vector3f, 3> &v)
    -> std::tuple<float, float, float> {
  float c1 =
      (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y +
       v[1].x() * v[2].y() - v[2].x() * v[1].y()) /
      (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() +
       v[1].x() * v[2].y() - v[2].x() * v[1].y());
  float c2 =
      (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y +
       v[2].x() * v[0].y() - v[0].x() * v[2].y()) /
      (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() +
       v[2].x() * v[0].y() - v[0].x() * v[2].y());
  float c3 =
      (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y +
       v[0].x() * v[1].y() - v[1].x() * v[0].y()) /
      (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() +
       v[0].x() * v[1].y() - v[1].x() * v[0].y());
  return {c1, c2, c3};
}

// set frame buffer size and depth buffer size
rst::rasterizer::rasterizer(int w, int h) : width(w), height(h) {
  frame_buf.resize(w * h);
  depth_buf.resize(w * h);
}
// mapping
auto rst::rasterizer::load_positions(
    const std::vector<Eigen::Vector3f> &positions) -> rst::pos_buf_id {
  auto id = get_next_id();
  pos_buf.emplace(id, positions);

  return {id};
}
auto rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
    -> rst::ind_buf_id {
  auto id = get_next_id();
  ind_buf.emplace(id, indices);

  return {id};
}
auto rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
    -> rst::col_buf_id {
  auto id = get_next_id();
  col_buf.emplace(id, cols);

  return {id};
}
// set MVP Matrix
void rst::rasterizer::set_model(const Eigen::Matrix4f &m) { model = m; }
void rst::rasterizer::set_view(const Eigen::Matrix4f &v) { view = v; }
void rst::rasterizer::set_projection(const Eigen::Matrix4f &p) {
  projection = p;
}
// 将点转换为一维坐标后设置点的颜色
void rst::rasterizer::set_pixel(const Eigen::Vector3f &point,
                                const Eigen::Vector3f &color) {
  auto ind = (height - 1 - point.y()) * width + point.x();
  frame_buf[ind] = color;
}
// 区分 color 和 color 后清空缓存数组
void rst::rasterizer::clear(Buffers buff) {
  if ((buff & rst::Buffers::Color) == rst::Buffers::Color) {
    std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
  }
  if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth) {
    std::fill(depth_buf.begin(), depth_buf.end(),
              std::numeric_limits<float>::infinity());
  }
}
//
void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer,
                           col_buf_id col_buffer, Primitive type) {
  // get value in map
  auto &buf = pos_buf[pos_buffer.pos_id];
  auto &ind = ind_buf[ind_buffer.ind_id];
  auto &col = col_buf[col_buffer.col_id];
  float f1 = (50 - 0.1) / 2.0;
  float f2 = (50 + 0.1) / 2.0;
  // MVP transformation
  Eigen::Matrix4f mvp = projection * view * model;
  for (auto &i : ind) {
    Triangle t;
    std::array<Eigen::Vector4f, 3> v = {mvp * to_vec4(buf[i[0]], 1.0f),
                                        mvp * to_vec4(buf[i[1]], 1.0f),
                                        mvp * to_vec4(buf[i[2]], 1.0f)};
    // Homogeneous division
    for (auto &vec : v) {
      vec /= vec.w();
    }
    // Viewport transformation
    for (auto &vert : v) {
      vert.x() = 0.5 * width * (vert.x() + 1.0);
      vert.y() = 0.5 * height * (vert.y() + 1.0);
      vert.z() = vert.z() * f1 + f2;
    }

    for (int i = 0; i < 3; ++i) {
      t.setVertex(i, v[i].head<3>());
      t.setVertex(i, v[i].head<3>());
      t.setVertex(i, v[i].head<3>());
    }

    auto col_x = col[i[0]];
    auto col_y = col[i[1]];
    auto col_z = col[i[2]];

    t.setColor(0, col_x[0], col_x[1], col_x[2]);
    t.setColor(1, col_y[0], col_y[1], col_y[2]);
    t.setColor(2, col_z[0], col_z[1], col_z[2]);

    rasterize_triangle(t);
  }
}

auto rst::rasterizer::frame_buffer() -> std::vector<Eigen::Vector3f> & {
  return frame_buf;
}
//
void rst::rasterizer::rasterize_triangle(const Triangle &t) {
  auto v = t.toVector4();
  double minx = 0, miny = 0, maxx = 0, maxy = 0;
  for (int i = 0; i < 3; i++) {
    const Vector3f &point = t.v[i];
    if (!i) {
      minx = maxx = point.x();
      miny = maxy = point.y();
      continue;
    }
    minx = point.x() < minx ? point.x() : minx;
    miny = point.y() < miny ? point.y() : miny;
    maxx = point.x() > maxx ? point.x() : maxx;
    maxy = point.y() > maxy ? point.y() : maxy;
  }

  // iterate through the pixel and find if the current pixel is inside the

  for (int x = (int)minx; x < maxx; x++) {
    for (int y = (int)miny; y < maxy; y++) {

      if (!insideTriangle(x, y, t.v))
        continue;
      // If so, use the following code to get the interpolated z value.
      auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
      float w_reciprocal =
          1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
      float z_interpolated = alpha * v[0].z() / v[0].w() +
                             beta * v[1].z() / v[1].w() +
                             gamma * v[2].z() / v[2].w();
      z_interpolated *= w_reciprocal;

      int buf_index = get_index(x, y);

      if (z_interpolated >= depth_buf[buf_index])
        continue;

      depth_buf[buf_index] = z_interpolated;

      set_pixel(Vector3f(x, y, 1), t.getColor());
    }
  }
}

auto rst::rasterizer::get_index(int x, int y) -> int {
  return (height - 1 - y) * width + x;
}

auto rst::rasterizer::get_next_id() -> int { return next_id++; }