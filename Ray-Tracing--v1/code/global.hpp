#ifndef GROBAL_HPP
#define GROBAL_HPP

#include <cmath>
#include <iostream>
#include <memory>
#include <random>
// Usings

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline auto degrees_to_radians(double degrees) -> double {
  return degrees * pi / 180.0;
}

inline auto random_double() -> double {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}
inline auto random_double(double min, double max) -> double {
  static std::mt19937 generator;
  static std::uniform_real_distribution<double> distribution(min, max);
  return distribution(generator);
}

inline auto clamp(const double &x, const double &min, const double &max)
    -> double {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

// 求根公式
inline auto solveQuadratic(const double &a, const double &b, const double &c,
                           double &x0, double &x1) -> bool {
  double discr = b * b - 4 * a * c;
  if (discr < 0)
    return false;
  else if (discr == 0)
    x0 = x1 = -0.5 * b / a;
  else {
    double q =
        (b > 0) ? -0.5 * (b + std::sqrt(discr)) : -0.5 * (b - std::sqrt(discr));
    x0 = q / a;
    x1 = c / q;
  }
  if (x0 > x1)
    std::swap(x0, x1);
  return true;
}

inline auto solveQuadratic_halfb(const double &a, const double &half_b,
                                 const double &c, double &x0, double &x1)
    -> bool {
  double discr = half_b * half_b - a * c;
  if (discr < 0)
    return false;
  else if (discr == 0)
    x0 = x1 = -half_b / a;
  else {
    double q = -half_b + std::sqrt(discr);
    x0 = q / a;
    x1 = c / q;
  }
  if (x0 > x1)
    std::swap(x0, x1);
  return true;
}

inline void UpdateProgress(double progress) {
  int barWidth = 100;

  std::cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %\r";
  std::cout.flush();
}

inline void UpdateProgress(double now, double total) {
  int barWidth = 100;
  double progress = now / total;
  std::cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %\r";
  std::cout.flush();
}

#endif