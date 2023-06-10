/*
  Created by sy 2023/6/5
*/

#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "Object.hpp"

#include <cstring>
#include <memory>
/*

*/
class MeshTriangle : public Object {
public:
  uint32_t numTriangles;
  std::vector<Vector3f> vertices;
  std::vector<uint32_t> vertexIndex;
  std::vector<Vector2f> stCoordinates;

public:
  MeshTriangle(std::vector<Vector3f> &verts, std::vector<uint32_t> &vertsIndex,
               const uint32_t &numTris, std::vector<Vector2f> &st);

  auto intersect(const Vector3f &orig, const Vector3f &dir, float &tnear,
                 uint32_t &index, Vector2f &uv) const -> bool override;
  void getSurfaceProperties(const Vector3f &, const Vector3f &,
                            const uint32_t &index, const Vector2f &uv,
                            Vector3f &N, Vector2f &st) const override;
  [[nodiscard]] auto evalDiffuseColor(const Vector2f &st) const
      -> Vector3f override;
};

#endif