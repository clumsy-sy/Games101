#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cstdint>
#include <functional>
#include <vector>
#include <future>
#include <utility>

#include "../external/BMP.hpp"
#include "../camera/camera.hpp"
#include "../geometry/hittablelist.hpp"
#include "../material/material.hpp"

auto ray_color(const ray &r, const hittable &world, int depth) -> color;

class Renderer {
public:
  std::string photoname = "image.bmp";
  camera cam;
  hittable_list world;
  double aspect_ratio = 16.0 / 9.0;
  uint32_t image_width = 1200;
  uint32_t image_height = static_cast<uint32_t>(image_width / aspect_ratio);
  uint32_t samples_per_pixel = 64; // 单素采样数
  uint32_t max_depth = 50;         // 光线递归深度
  uint32_t async_num = 36;         // 线程数

public:
  Renderer() = default;
  Renderer(hittable_list &hitlist) : world(hitlist) {
    cam = camera();
  };
  Renderer(hittable_list &hitlist, double ratio, uint32_t width)
      : world(hitlist), aspect_ratio(ratio), image_width(width) {
    image_height = static_cast<uint32_t>(image_width / aspect_ratio);
  }
  auto set_camera(camera &c) {
    cam = c;
  }
  auto set_photo_name(std::string name) {
    photoname = std::move(name);
  }
  auto set_samples_per_pixel(uint32_t samples) {
    samples_per_pixel = samples;
  }
  auto set_max_depth(uint32_t depth) {
    max_depth = depth;
  }
  auto set_async_num(uint32_t num) {
    async_num = num;
  }
  auto render() {
    bmp::bitmap photo(image_width, image_height); // photo
    std::vector<std::future<void>> deque;         // thread deque
    std::mutex cout_mutex;                        // lock the cnt and std::cout
    std::int32_t cnt = 0;
    // 开始
    UpdateProgress(cnt, image_height - 1);
    
    auto action = [&](uint32_t jl, uint32_t jr) -> void {
      for (uint32_t j = jl; j < jr; ++j) {
        for (uint32_t i = 0; i < image_width; ++i) {
          color pixel_color(0, 0, 0);
          for (uint32_t s = 0; s < samples_per_pixel; ++s) {
            auto u = (i + random_double()) / (image_width - 1);
            auto v = (j + random_double()) / (image_height - 1);
            ray r = cam.get_ray(u, v);
            pixel_color += ray_color(r, world, max_depth);
          }
          photo.set(i, j, pixel_color, samples_per_pixel);
        }
        cout_mutex.lock();
        UpdateProgress(++cnt, image_height - 1);
        cout_mutex.unlock();
      }
    };

    uint32_t block = image_height / async_num;
    for (uint32_t ti = 0; ti != async_num; ++ti) {
      uint32_t jl = ti * block, jr = (ti + 1) * block;
      if (ti + 1 == async_num)
        jr = image_height;
      deque.emplace_back(std::async(std::launch::async, action, jl, jr));
    }

    for (auto &i : deque) {
      i.wait();
    }

    photo.generate(photoname);
  }
  auto simple_random_sampling(uint32_t i, uint32_t j, uint32_t times) -> color {
    color res(0, 0, 0);
    for (uint32_t s = 0; s < times; ++s) {
      auto u = (i + random_double()) / (image_width - 1);
      auto v = (j + random_double()) / (image_height - 1);
      ray r = cam.get_ray(u, v);
      res += ray_color(r, world, max_depth);
    }
    return res;
  }
};


auto ray_color(const ray &r, const hittable &world, int depth) -> color {
  if (depth <= 0)
    return {0, 0, 0};
  hit_record rec;
  if (world.hit(r, 0.0001, infinity, rec)) {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return {0, 0, 0};
  }
  Vec3d unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

#endif