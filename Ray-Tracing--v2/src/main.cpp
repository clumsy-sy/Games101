#include "main.hpp"
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
  point3 lookfrom(20, 5, 5);

  point3 lookat(0, 0, 0);
  Vec3d vup(0, 1, 0);
  // auto dist_to_focus = 10.0
  auto dist_to_focus = (lookfrom - lookat).length();
  auto aperture = 0.1;

  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
  // Renderer
  Renderer Render(world, aspect_ratio, image_width);
  Render.set_camera(cam);
  Render.set_photo_name(photoname);
  Render.set_samples_per_pixel(256);
  Render.render();

  return 0;
}
