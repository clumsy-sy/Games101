#include "BMP.hpp"
#include "global.hpp"
auto main() -> int {
  // Image
  const int image_width = 256;
  const int image_height = 256;

  // Render
  bmp::bitmap photo(image_width, image_height);
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      auto r = double(i) / (image_width - 1);
      auto g = double(j) / (image_height - 1);
      auto b = 0.25;
      photo.set(i, j, {r, g, b});
    }
    UpdateProgress(j, image_height - 1);
  }
  photo.generate("image1.bmp");

  return 0;
}