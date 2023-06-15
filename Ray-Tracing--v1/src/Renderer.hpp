#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <functional>
#include <vector>
#include <future>
#include <utility>

#include "BMP.hpp"
#include "camera.hpp"
#include "ray_color.hpp"
#include "hittablelist.hpp"

class Renderer {
public:
  std::string photoname = "image.bmp";
  camera cam;
  hittable_list world;
  double aspect_ratio = 16.0 / 9.0;
  int image_width = 1200;
  int image_height = static_cast<int>(image_width / aspect_ratio);
  int samples_per_pixel = 500;
  int max_depth = 50;

public:
  Renderer() = default;
  Renderer(hittable_list &hitlist) : world(hitlist) {
    cam = camera();
  };
  Renderer(hittable_list &hitlist, double ratio, int width) : world(hitlist), aspect_ratio(ratio), image_width(width) {
    image_height = static_cast<int>(image_width / aspect_ratio);
  }
  auto set_camera(camera &c) {
    cam = c;
  }
  auto set_photo_name(std::string name) {
    photoname = std::move(name);
  }
  auto set_samples_per_pixel(int samples) {
    samples_per_pixel = samples;
  }
  auto set_max_depth(int depth) {
    max_depth = depth;
  }
  auto render() {
    bmp::bitmap photo(image_width, image_height); // photo
    std::vector<std::future<void>> deque;         // thread deque
    std::mutex cout_mutex;                        // lock the cnt and std::cout
    std::int32_t cnt = 0;

    auto action = [&](int jl, int jr) -> void {
      for (int j = jl; j < jr; ++j) {
        for (int i = 0; i < image_width; ++i) {
          color pixel_color(0, 0, 0);
          for (int s = 0; s < samples_per_pixel; ++s) {
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

    uint32_t async_num = 40;
    uint32_t block = image_height / async_num;
    for (uint32_t ti = 0; ti != async_num; ++ti) {
      int jl = ti * block, jr = (ti + 1) * block;
      if (ti + 1 == async_num)
        jr = image_height;
      deque.emplace_back(std::async(std::launch::async, action, jl, jr));
    }

    for (auto &i : deque) {
      i.wait();
    }

    photo.generate(photoname);
  }
};

#endif