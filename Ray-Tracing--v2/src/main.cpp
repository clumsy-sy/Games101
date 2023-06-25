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
  uint32_t samples_per_pixel = 400;
  point3 lookfrom, lookat;
  auto vfov = 40.0;
  auto aperture = 0.0;
  color background(0, 0, 0);

  switch (0) {
  case 1:
    world = random_scene();
    background = color(0.70, 0.80, 1.00);
    lookfrom = point3(13, 2, 3);
    lookat = point3(0, 0, 0);
    vfov = 20.0;
    aperture = 0.1;
    break;

  case 2:
    world = two_spheres();
    background = color(0.70, 0.80, 1.00);
    lookfrom = point3(13, 2, 3);
    lookat = point3(0, 0, 0);
    vfov = 20.0;
    break;

  case 3:
    world = two_perlin_spheres();
    background = color(0.70, 0.80, 1.00);
    lookfrom = point3(13, 2, 3);
    lookat = point3(0, 0, 0);
    vfov = 20.0;
    break;

  case 4:
    world = earth();
    background = color(0.70, 0.80, 1.00);
    lookfrom = point3(13, 2, 3);
    lookat = point3(0, 0, 0);
    vfov = 20.0;
    break;
  default:
  case 5:
    world = simple_light();
    samples_per_pixel = 400;
    background = color(0, 0, 0);
    lookfrom = point3(26, 3, 6);
    lookat = point3(0, 2, 0);
    vfov = 20.0;
    break;
  }

  // Camera

  Vec3d vup(0, 1, 0);
  // auto dist_to_focus = 10.0;
  auto dist_to_focus = (lookfrom - lookat).length();

  camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  // Renderer
  Renderer Render(world, aspect_ratio, image_width);
  Render.set_camera(cam);
  Render.set_photo_name(photoname);
  Render.set_samples_per_pixel(samples_per_pixel);
  Render.set_background(background);
  Render.render();

  return 0;
}
