#include "main.hpp"
#include "world.hpp"

auto main(int argc, const char **argv) -> int {
  std::string photoname = "image.bmp";
  if (argc >= 2) {
    photoname = std::string(argv[1]);
  }

  hittable_list world;
  auto aspect_ratio = 16.0 / 9.0;
  int image_width = 1200;

  point3 lookfrom, lookat;
  auto vfov = 40.0;
  auto aperture = 0.0;

  switch (0) {
  case 1:
    world = random_scene();
    lookfrom = point3(13, 2, 3);
    lookat = point3(0, 0, 0);
    vfov = 20.0;
    aperture = 0.1;
    break;

  default:
  case 2:
    world = two_spheres();
    lookfrom = point3(13, 2, 3);
    lookat = point3(0, 0, 0);
    vfov = 20.0;
    break;
  }

  // Camera

  Vec3d vup(0, 1, 0);
  // auto dist_to_focus = 10.0;
  auto dist_to_focus = (lookfrom - lookat).length();
  int image_height = static_cast<int>(image_width / aspect_ratio);

  camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  // Renderer
  Renderer Render(world, aspect_ratio, image_width);
  Render.set_camera(cam);
  Render.set_photo_name(photoname);
  Render.set_samples_per_pixel(400);
  Render.render();

  return 0;
}
