#ifndef SCENE_HPP
#define SCENE_HPP

#include "../geometry/hittablelist.hpp"
#include "balls_world.hpp"
#include "checker_balls.hpp"
#include "earth.hpp"

inline auto choose_scene(uint32_t opt, point3 &lookfrom, point3 &lookat, color &background) -> hittable_list {
  lookfrom = point3(13, 2, 3);
  // lookfrom = point3(4, 4, 20);
  lookat = point3(0, 0, 0);
  background = color(0.70, 0.80, 1.00);

  switch (opt) {
  case 1:
    return random_balls_world();
  case 2:
    background = color(0.70, 0.80, 1.00);
    return random_checkerANDballs();
  case 3:
    return earth();
  default:
    return random_checkerANDballs();
  }
}

inline auto choose_scene(uint32_t opt) -> hittable_list {

  switch (opt) {
  case 1:
    return random_balls_world();
  case 2:
    return random_checkerANDballs();
  case 3:
    return earth();
  default:
    return random_checkerANDballs();
  }
}

#endif