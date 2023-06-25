#ifndef WORLD_HPP
#define WORLD_HPP

#include "dielectric.hpp"
#include "hittablelist.hpp"
#include "lambertian.hpp"
#include "metal.hpp"
#include "sphere.hpp"
#include "moving_sphere.hpp"

auto random_scene() -> hittable_list {
  hittable_list world;

  auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

  // auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
  // world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = color::random() * color::random();
          sphere_material = std::make_shared<lambertian>(albedo);
          auto center2 = center + Vec3d(0, random_double(0, .5)(), 0);
          world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5)();
          sphere_material = std::make_shared<metal>(albedo, fuzz);
          world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = std::make_shared<dielectric>(1.5);
          world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = std::make_shared<dielectric>(1.5);
  world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
  world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  return world;
}

auto two_spheres() -> hittable_list {
  hittable_list objects;

  auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

  objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
  objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

  return objects;
}

auto two_perlin_spheres() -> hittable_list {
  hittable_list objects;

  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
  objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

  return objects;
}

auto earth() -> hittable_list {
  auto earth_texture = make_shared<image_texture>("build/earthmap.jpg");
  auto earth_surface = make_shared<lambertian>(earth_texture);
  auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

  return hittable_list(globe);
}

#endif