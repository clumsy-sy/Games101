#include "main.hpp"
#include "global.hpp"
#include "world.hpp"

auto main(int argc, const char **argv) -> int {
  std::string photoname = "image.bmp";
  if (argc >= 2) {
    photoname = std::string(argv[1]);
  }

  // World
  auto world = random_scene();
  const int image_width = 1200;

  // Camera
  const auto aspect_ratio = 3.0 / 2.0;
  // point3 lookfrom(13, 2, 3);
  point3 lookfrom(10, 5, 5);

  point3 lookat(0, 0, 0);
  Vec3d vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;

  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
  // Renderer
  Renderer Render(world, aspect_ratio, image_width);
  Render.set_camera(cam);
  Render.set_photo_name(photoname);
  Render.set_samples_per_pixel(400);
  Render.render();

  return 0;
}

// #include <immintrin.h>
// #include <new>
// #include <span>

// auto sum(std::vector<double> &a) -> double {
//   double ret = 0;
//   for (int i = 0; i < a.size(); ++i)
//     ret += a[i];
//   return ret;
// }

// auto dots(std::span<double> a, std::span<double> b) {
//   // a *= b
//   double ret = 0;
//   // using f64x8 [[gnu::vector_size(64)]] = double;
//   int num = a.size();
//   // auto *ax8 = (f64x8 *)a.data(), *bx8 = (f64x8 *)b.data();
//   int n8 = num / 8;
//   // for (int i = 0; i < n8; ++i)
//   //   ax8[i] *= bx8[i];
//   for (int i = 0; i < n8; ++i) {
//     auto aix8 = _mm256_load_pd(&a[i * 8]);
//     auto bix8 = _mm256_load_pd(&b[i * 8]);
//     aix8 = _mm256_mul_pd(aix8, bix8);
//     _mm256_store_pd(&a[i * 8], aix8);
//   }
//   for (int i = n8 * 8; i < num; ++i)
//     a[i] *= b[i];
// }

// auto main() -> int {

//   uint n = 32;
//   // std::vector<double> a(n), b(n);
//   auto _a = new (std::align_val_t(32)) double[n];
//   auto _b = new (std::align_val_t(32)) double[n];
//   std::span<double> a = {_a, n};
//   std::span<double> b = {_b, n};
//   for (int i = 0; i < n; ++i) {
//     a[i] = random_double();
//     b[i] = random_double();
//   }
//   for (auto ai : a) {
//     std::cout << ai << ' ';
//   }
//   std::cout << std::endl;
//   for (auto bi : b) {
//     std::cout << bi << ' ';
//   }
//   std::cout << std::endl;
//   dots(a, b);
//   for (auto ai : a) {
//     std::cout << ai << ' ';
//   }
//   std::cout << std::endl;
//   return 0;
// }