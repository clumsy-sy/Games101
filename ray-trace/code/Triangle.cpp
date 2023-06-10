/*
  Created by sy 2023/6/5
*/

#include "Triangle.hpp"
#include "Vector.hpp"

#include <memory>

auto rayTriangleIntersect(const Vector3f &v0, const Vector3f &v1,
                          const Vector3f &v2, const Vector3f &orig,
                          const Vector3f &dir, float &tnear, float &u, float &v)
    -> bool {
  // TODO: Implement this function that tests whether the triangle
  // that's specified bt v0, v1 and v2 intersects with the ray (whose
  // origin is *orig* and direction is *dir*)
  // Also don't forget to update tnear, u and v.
  auto O = orig, D = dir, P0 = v0, P1 = v1, P2 = v2;
  auto E1 = P1 - P0, E2 = P2 - P0, S = O - P0;
  auto S1 = crossProduct(D, E2), S2 = crossProduct(S, E1);
  tnear = dotProduct(S2, E2) / dotProduct(S1, E1);
  u = dotProduct(S1, S) / dotProduct(S1, E1);
  v = dotProduct(S2, D) / dotProduct(S1, E1);
  double eps = -1e-6;
  if (tnear > eps && u > eps && v > eps && 1 - u - v > eps)
    return true;
  return false;
  return false;
}

MeshTriangle::MeshTriangle(std::vector<Vector3f> &verts,
                           std::vector<uint32_t> &vertsIndex,
                           const uint32_t &numTris, std::vector<Vector2f> &st) {
  uint32_t maxIndex = 0;
  for (uint32_t i = 0; i < numTris * 3; ++i)
    if (vertsIndex[i] > maxIndex)
      maxIndex = vertsIndex[i];
  maxIndex += 1;
  vertices.resize(maxIndex);
  std::copy(verts.begin(), verts.end(), vertices.begin());
  vertexIndex.resize(numTris * 3);
  std::copy(vertsIndex.begin(), vertsIndex.end(), vertexIndex.begin());
  numTriangles = numTris;
  stCoordinates.resize(maxIndex);
  std::copy(st.begin(), st.end(), stCoordinates.begin());
}

auto MeshTriangle::intersect(const Vector3f &orig, const Vector3f &dir,
                             float &tnear, uint32_t &index, Vector2f &uv) const
    -> bool {
  bool intersect = false;
  for (uint32_t k = 0; k < numTriangles; ++k) {
    const Vector3f &v0 = vertices[vertexIndex[k * 3]];
    const Vector3f &v1 = vertices[vertexIndex[k * 3 + 1]];
    const Vector3f &v2 = vertices[vertexIndex[k * 3 + 2]];
    float t, u, v;
    if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear) {
      tnear = t;
      uv.x = u;
      uv.y = v;
      index = k;
      intersect |= true;
    }
  }

  return intersect;
}

void MeshTriangle::getSurfaceProperties(const Vector3f &, const Vector3f &,
                                        const uint32_t &index,
                                        const Vector2f &uv, Vector3f &N,
                                        Vector2f &st) const {
  const Vector3f &v0 = vertices[vertexIndex[index * 3]];
  const Vector3f &v1 = vertices[vertexIndex[index * 3 + 1]];
  const Vector3f &v2 = vertices[vertexIndex[index * 3 + 2]];
  Vector3f e0 = normalize(v1 - v0);
  Vector3f e1 = normalize(v2 - v1);
  N = normalize(crossProduct(e0, e1));
  const Vector2f &st0 = stCoordinates[vertexIndex[index * 3]];
  const Vector2f &st1 = stCoordinates[vertexIndex[index * 3 + 1]];
  const Vector2f &st2 = stCoordinates[vertexIndex[index * 3 + 2]];
  st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
}

auto MeshTriangle::evalDiffuseColor(const Vector2f &st) const -> Vector3f {
  float scale = 5;
  float pattern =
      (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
  return lerp(Vector3f(0.815, 0.235, 0.031), Vector3f(0.937, 0.937, 0.231),
              pattern);
}