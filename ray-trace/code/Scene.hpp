/*
  Created by sy on 2023/6/9
*/
#ifndef SCENE_HPP
#define SCENE_HPP
#include "Light.hpp"
#include "Object.hpp"
#include "Vector.hpp"
#include <memory>
#include <vector>

class Scene {
private:
  // creating the scene (adding objects and lights)
  std::vector<std::unique_ptr<Object>> objects;
  std::vector<std::unique_ptr<Light>> lights;

public:
  // setting up options 长宽 fov，背景颜色，最大递归深度，eps
  int width = 1280;
  int height = 960;
  double fov = 90;
  Vector3f backgroundColor = Vector3f(0.235294, 0.67451, 0.843137);
  int maxDepth = 5;
  float epsilon = 0.00001;

public:
  Scene(int w, int h) : width(w), height(h) {}

  void Add(std::unique_ptr<Object> object) {
    objects.push_back(std::move(object));
  }
  void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

  [[nodiscard]] auto get_objects() const
      -> const std::vector<std::unique_ptr<Object>> & {
    return objects;
  }
  [[nodiscard]] auto get_lights() const
      -> const std::vector<std::unique_ptr<Light>> & {
    return lights;
  }
};
#endif