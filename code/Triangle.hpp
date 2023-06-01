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
  std::array<Vector4f, 3> v; /*the coordinates of the triangle, v0, v1, v2 in
                                counter clockwise order*/
  /*Per vertex values*/
  std::array<Vector3f, 3> color;      // color at each vertex;
  std::array<Vector2f, 3> tex_coords; // texture u,v
  std::array<Vector3f, 3> normal;     // normal vector for each vertex

  // Texture *tex;
  Triangle();

  [[nodiscard]] auto a() const -> Eigen::Vector3f;
  [[nodiscard]] auto b() const -> Eigen::Vector3f;
  [[nodiscard]] auto c() const -> Eigen::Vector3f;

  void setVertex(int ind, Vector4f ver); /*set i-th vertex coordinates */
  void setNormal(int ind, Vector3f n);   /*set i-th vertex normal vector*/
  void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/
  [[nodiscard]] auto getColor() const
      -> Vector3f; // Only one color per triangle.
  void setTexCoord(int ind, float s,
                   float t); /*set i-th vertex texture coordinate*/
  void setTexCoord(int ind, Eigen::Vector2f tex);
  [[nodiscard]] auto toVector4() const
      -> std::array<Vector4f, 3>; // to vector4f(homogeneous coordinates)
};

#endif // TRIANGLE_H