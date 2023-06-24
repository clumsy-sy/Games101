#ifndef PERLIN_HPP
#define PERLIN_HPP

#include "Vec3dx4.hpp"
#include "global.hpp"

class perlin {
private:
  static const int point_count = 256;
  Vec3d *ranvec;
  int *perm_x;
  int *perm_y;
  int *perm_z;

  static auto perlin_generate_perm() -> int * {
    auto p = new int[point_count];

    for (int i = 0; i < perlin::point_count; i++)
      p[i] = i;

    permute(p, point_count);

    return p;
  }

  static void permute(int *p, int n) {
    for (int i = n - 1; i > 0; i--) {
      int target = random_int(0, i)();
      int tmp = p[i];
      p[i] = p[target];
      p[target] = tmp;
    }
  }
  static auto trilinear_interp(double c[2][2][2], double u, double v, double w) -> double {
    auto accum = 0.0;
    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++)
          accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) * (k * w + (1 - k) * (1 - w)) * c[i][j][k];

    return accum;
  }
  static auto perlin_interp(Vec3d c[2][2][2], double u, double v, double w) -> double {
    auto uu = u * u * (3 - 2 * u);
    auto vv = v * v * (3 - 2 * v);
    auto ww = w * w * (3 - 2 * w);
    auto accum = 0.0;

    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++) {
          Vec3d weight_v(u - i, v - j, w - k);
          accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) *
                   dot(c[i][j][k], weight_v);
        }

    return accum;
  }

public:
  perlin() {
    ranvec = new Vec3d[point_count];
    for (int i = 0; i < point_count; ++i) {
      ranvec[i] = unit_vector(Vec3d::random(-1, 1));
    }

    perm_x = perlin_generate_perm();
    perm_y = perlin_generate_perm();
    perm_z = perlin_generate_perm();
  }

  ~perlin() {
    delete[] ranvec;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
  }

  [[nodiscard]] auto noise(const point3 &p) const -> double {
    auto u = p.x() - floor(p.x());
    auto v = p.y() - floor(p.y());
    auto w = p.z() - floor(p.z());
    auto i = static_cast<int>(floor(p.x()));
    auto j = static_cast<int>(floor(p.y()));
    auto k = static_cast<int>(floor(p.z()));
    Vec3d c[2][2][2];

    for (int di = 0; di < 2; di++)
      for (int dj = 0; dj < 2; dj++)
        for (int dk = 0; dk < 2; dk++)
          c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];

    return perlin_interp(c, u, v, w);
  }
};

#endif