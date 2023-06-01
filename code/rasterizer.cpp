/*
        Created by sy on 2023/5/29
*/

#include "rasterizer.hpp"

#include <utility>

// vec3 to vec4
auto to_vec4(const Eigen::Vector3f &v3, float w = 1.0f) {
  return Vector4f(v3.x(), v3.y(), v3.z(), w);
}
// judge a point is inside the triangle?
static auto insideTriangle(int x, int y,
                           const std::array<Eigen::Vector4f, 3> &_v) -> bool {
  const Vector3f A = {_v[0].x(), _v[0].y(), 1.0f};
  const Vector3f B = {_v[1].x(), _v[1].y(), 1.0f};
  const Vector3f C = {_v[2].x(), _v[2].y(), 1.0f};

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
                                 const std::array<Eigen::Vector4f, 3> &v)
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
// 插值
static auto interpolate(float alpha, float beta, float gamma,
                        const Eigen::Vector3f &vert1,
                        const Eigen::Vector3f &vert2,
                        const Eigen::Vector3f &vert3, float weight)
    -> Eigen::Vector3f {
  return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}
static auto interpolate(float alpha, float beta, float gamma,
                        const Eigen::Vector2f &vert1,
                        const Eigen::Vector2f &vert2,
                        const Eigen::Vector2f &vert3, float weight)
    -> Eigen::Vector2f {
  auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
  auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

  u /= weight;
  v /= weight;

  return {u, v};
}

// set frame buffer size and depth buffer size
rst::rasterizer::rasterizer(int w, int h) : width(w), height(h) {
  frame_buf.resize(w * h);
  depth_buf.resize(w * h);
  texture = std::nullopt;
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
auto rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &colors)
    -> rst::col_buf_id {
  auto id = get_next_id();
  col_buf.emplace(id, colors);

  return {id};
}
auto rst::rasterizer::load_normals(const std::vector<Eigen::Vector3f> &mormals)
    -> rst::nor_buf_id {
  auto id = get_next_id();
  nor_buf.emplace(id, mormals);

  return {id};
}
// set MVP Matrix
void rst::rasterizer::set_model(const Eigen::Matrix4f &m) { model = m; }
void rst::rasterizer::set_view(const Eigen::Matrix4f &v) { view = v; }
void rst::rasterizer::set_projection(const Eigen::Matrix4f &p) {
  projection = p;
}
//
// inline void rst::rasterizer::set_texture(Texture tex) { texture = tex; }
//
void rst::rasterizer::set_vertex_shader(
    std::function<Eigen::Vector3f(vertex_shader_payload)> vert_shader) {
  vertex_shader = std::move(vert_shader);
}
void rst::rasterizer::set_fragment_shader(
    std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader) {
  fragment_shader = std::move(frag_shader);
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
      t.setVertex(i, v[i].head<4>());
      t.setVertex(i, v[i].head<4>());
      t.setVertex(i, v[i].head<4>());
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

void rst::rasterizer::draw(std::vector<Triangle *> &TriangleList) {
  float f1 = (50 - 0.1) / 2.0;
  float f2 = (50 + 0.1) / 2.0;

  Eigen::Matrix4f mvp = projection * view * model;
  for (const auto &t : TriangleList) {
    Triangle newtriangle = *t;

    std::array<Eigen::Vector4f, 3> mm{(view * model * t->v[0]),
                                      (view * model * t->v[1]),
                                      (view * model * t->v[2])};

    std::array<Eigen::Vector3f, 3> viewspace_pos;

    std::transform(mm.begin(), mm.end(), viewspace_pos.begin(),
                   [](auto &v) { return v.template head<3>(); });

    std::array<Eigen::Vector4f, 3> v = {mvp * t->v[0], mvp * t->v[1],
                                        mvp * t->v[2]};
    // Homogeneous division
    for (auto &vec : v) {
      vec.x() /= vec.w();
      vec.y() /= vec.w();
      vec.z() /= vec.w();
    }

    Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
    std::array<Eigen::Vector4f, 3> n = {inv_trans * to_vec4(t->normal[0], 0.0f),
                                        inv_trans * to_vec4(t->normal[1], 0.0f),
                                        inv_trans *
                                            to_vec4(t->normal[2], 0.0f)};

    // Viewport transformation
    for (auto &vert : v) {
      vert.x() = 0.5 * width * (vert.x() + 1.0);
      vert.y() = 0.5 * height * (vert.y() + 1.0);
      vert.z() = vert.z() * f1 + f2;
    }

    for (int i = 0; i < 3; ++i) {
      // screen space coordinates
      newtriangle.setVertex(i, v[i]);
    }

    for (int i = 0; i < 3; ++i) {
      // view space normal
      newtriangle.setNormal(i, n[i].head<3>());
    }

    newtriangle.setColor(0, 148, 121.0, 92.0);
    newtriangle.setColor(1, 148, 121.0, 92.0);
    newtriangle.setColor(2, 148, 121.0, 92.0);

    // Also pass view space vertice position
    rasterize_triangle(newtriangle, viewspace_pos);
  }
}

auto rst::rasterizer::frame_buffer() -> std::vector<Eigen::Vector3f> & {
  return frame_buf;
}
// Bresenham's line drawing algorithm
void rst::rasterizer::draw_line(Eigen::Vector3f begin, Eigen::Vector3f end) {
  // auto x1 = begin.x(), y1 = begin.y();
  // auto x2 = end.x(), y2 = end.y();

  // Eigen::Vector3f line_color = {255, 255, 255};

  // int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

  // dx = x2 - x1, dy = y2 - y1;
  // dx1 = fabs(dx), dy1 = fabs(dy);
  // px = 2 * dy1 - dx1, py = 2 * dx1 - dy1;

  // if (dy1 <= dx1) {
  //   if (dx >= 0) {
  //     x = x1;
  //     y = y1;
  //     xe = x2;
  //   } else {
  //     x = x2;
  //     y = y2;
  //     xe = x1;
  //   }
  //   Eigen::Vector2i point = Eigen::Vector2i(x, y);
  //   set_pixel(point, line_color);
  //   for (i = 0; x < xe; i++) {
  //     x = x + 1;
  //     if (px < 0) {
  //       px = px + 2 * dy1;
  //     } else {
  //       if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
  //         y = y + 1;
  //       } else {
  //         y = y - 1;
  //       }
  //       px = px + 2 * (dy1 - dx1);
  //     }
  //     //            delay(0);
  //     Eigen::Vector2i point = Eigen::Vector2i(x, y);
  //     set_pixel(point, line_color);
  //   }
  // } else {
  //   if (dy >= 0) {
  //     x = x1;
  //     y = y1;
  //     ye = y2;
  //   } else {
  //     x = x2;
  //     y = y2;
  //     ye = y1;
  //   }
  //   Eigen::Vector2i point = Eigen::Vector2i(x, y);
  //   set_pixel(point, line_color);
  //   for (i = 0; y < ye; i++) {
  //     y = y + 1;
  //     if (py <= 0) {
  //       py = py + 2 * dx1;
  //     } else {
  //       if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
  //         x = x + 1;
  //       } else {
  //         x = x - 1;
  //       }
  //       py = py + 2 * (dx1 - dy1);
  //     }
  //     //            delay(0);
  //     Eigen::Vector2i point = Eigen::Vector2i(x, y);
  //     set_pixel(point, line_color);
  //   }
  // }
}
//
void rst::rasterizer::rasterize_triangle(const Triangle &t) {
  auto v = t.toVector4();
  // AABB
  double minX, minY, maxX, maxY;
  minX = maxX = v[0][0];
  minX = maxX = v[0][1];
  for (int i = 1; i < 3; i++) { // 从第二个点个开始
    const Vector4f &point = t.v[i];

    minX = std::min((double)point.x(), minX);
    minY = std::min((double)point.y(), minY);

    maxX = std::max((double)point.x(), maxX);
    maxY = std::max((double)point.y(), maxY);
  }

  // iterate through the pixel and find if the current pixel is inside the

  for (int x = (int)minX; x <= (int)maxX; x++) {
    for (int y = (int)minY; y <= (int)maxY; y++) {

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
void rst::rasterizer::rasterize_triangle(const Triangle &t, const std::array<Eigen::Vector3f, 3> &world_pos) {
  auto v = t.toVector4();
  // AABB
  double minX, minY, maxX, maxY;
  minX = maxX = v[0][0];
  minX = maxX = v[0][1];
  for (int i = 1; i < 3; i++) { // 从第二个点个开始
    const Vector4f &point = t.v[i];

    minX = std::min((double)point.x(), minX);
    minY = std::min((double)point.y(), minY);

    maxX = std::max((double)point.x(), maxX);
    maxY = std::max((double)point.y(), maxY);
  }

  // iterate through the pixel and find if the current pixel is inside the

  for (int x = (int)minX; x <= (int)maxX; x++) {
    for (int y = (int)minY; y <= (int)maxY; y++) {

      if (!insideTriangle(x, y, t.v))
        continue;
      // If so, use the following code to get the interpolated z value.
      auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
      auto interpolated_color = alpha * t.color[0] + beta * t.color[1] + gamma * t.color[2];
      auto interpolated_normal = alpha * t.normal[0] + beta * t.normal[1] + gamma * t.normal[2];
      auto interpolated_texcoords = alpha * t.tex_coords[0] + beta * t.tex_coords[1] + gamma * t.tex_coords[2];
      auto interpolated_shadingcoords = alpha * world_pos[0] + beta * world_pos[1] + gamma * world_pos[2];
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

      fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ?
      &*texture : nullptr);
      payload.view_pos = interpolated_shadingcoords;
      auto pixel_color = fragment_shader(payload);

      set_pixel(Vector3f(x, y, 1), pixel_color);
    }
  }
}
// SSAA
void rst::rasterizer::rasterize_triangle_SSAA(const Triangle &t,
                                              const int N = 2) {
  auto v = t.toVector4();

  double minX, minY, maxX, maxY;
  minX = maxX = v[0][0];
  minX = maxX = v[0][1];
  for (int i = 1; i < 3; i++) {
    const Vector4f &point = t.v[i];

    minX = std::min((double)point.x(), minX);
    minY = std::min((double)point.y(), minY);

    maxX = std::max((double)point.x(), maxX);
    maxY = std::max((double)point.y(), maxY);
  }

  // iterate through the pixel and find if the current pixel is inside the

  for (int x = (int)minX; x <= (int)maxX; x++) {
    for (int y = (int)minY; y <= (int)maxY; y++) {
      // SSAA need N > 1
      int id = 0;
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
          float d = 1.0 / (2 * N);
          float nowX = x + 2 * d * i + d, nowY = y + 2 * d * j + d;
          if (insideTriangle(nowX, nowY, t.v)) {
            auto [alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
            float w_reciprocal =
                1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
            float z_interpolated = alpha * v[0].z() / v[0].w() +
                                   beta * v[1].z() / v[1].w() +
                                   gamma * v[2].z() / v[2].w();
            z_interpolated *= w_reciprocal;
            int buf_index = get_index(nowX, nowY);
            if (depth_buf_SSAA[buf_index][id] < z_interpolated) {
              depth_buf_SSAA[buf_index][id] = z_interpolated;
              color_buf_SSAA[buf_index][id] = t.getColor();
            }
          }
          id++;
        }
      }
      Eigen::Vector3f color = {0, 0, 0};
      id = 0;
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
          color += color_buf_SSAA[get_index(x, y)][id];
          id++;
        }
      }
      color /= N * N;
      set_pixel(Eigen::Vector3f(x, y, 1), color);
    }
  }
}

auto rst::rasterizer::get_index(int x, int y) -> int {
  return (height - 1 - y) * width + x;
}

auto rst::rasterizer::get_next_id() -> int { return next_id++; }