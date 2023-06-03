/*
  Created by sy 2023/5/29
*/

#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <array>
#include <eigen3/Eigen/Eigen>

using namespace Eigen;
/*
    一个三角形类，有顶点坐标（顺时针）、颜色、材质、顶点法向量
*/
class Triangle {
public:
  /*
    齐次坐标下三角形的信息
    1. 三个点的坐标 V0，V1，V2，1.0 顺时针
    2. 三个点的颜色信息（R，G，B）
    3. 三个点的材质信息 （U，V）
    4. 三个点的法向量信息（Vector3f）
  */
  std::array<Vector4f, 3> v; /*the coordinates of the triangle, v0, v1, v2, 1.0
                                in counter clockwise order*/
  std::array<Vector3f, 3> color;      // color at each vertex;
  std::array<Vector2f, 3> tex_coords; // texture u,v
  std::array<Vector3f, 3> normal;     // normal vector for each vertex
  // 构造函数和每个点的 get 函数
  Triangle();
  [[nodiscard]] auto a() const -> Eigen::Vector4f { return v[0]; };
  [[nodiscard]] auto b() const -> Eigen::Vector4f { return v[1]; };
  [[nodiscard]] auto c() const -> Eigen::Vector4f { return v[2]; };
  [[nodiscard]] auto getColor() const -> Vector3f { return color[0] * 255; };
  // set 函数，设置某个点的信息
  void setVertex(int ind, Vector4f ver); /*set i-th vertex coordinates */
  void setNormal(int ind, Vector3f n);   /*set i-th vertex normal vector*/
  void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/
  void setNormals(const std::array<Vector3f, 3> &normals);
  void setColors(const std::array<Vector3f, 3> &colors);
  void setTexCoord(int ind, float s,
                   float t); /*set i-th vertex texture coordinate*/
  void setTexCoord(int ind, Eigen::Vector2f uv);
  [[nodiscard]] auto toVector4() const
      -> std::array<Vector4f, 3>; // to vector4f(homogeneous coordinates)
};

#endif // TRIANGLE_H