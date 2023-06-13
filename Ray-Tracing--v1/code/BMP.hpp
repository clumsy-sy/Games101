/*
  Created by sy on 6/11/2023
*/

#ifndef BMP_HPP
#define BMP_HPP
/*
  输出 bmp 格式的图像 !! bmp 是（ b, g, r) 暂时只支持 24
  位格式，由于需要对齐，所以现在的宽度需要是 4 的倍数(偷懒了)
*/
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <span>
#include <string>
#include <vector>
#include "Vec3d.hpp"

namespace bmp {
struct BMP_FILE_HEADER {
  unsigned int bfSize;            /* Size of file */
  unsigned short bfReserved1 = 0; /* Reserved */
  unsigned short bfReserved2 = 0; /* ... */
  unsigned int bfOffBits = 0x36;  /* Offset to bitmap data */
};

struct BMP_INFO_HEADER {
  unsigned int biSize = sizeof(BMP_INFO_HEADER); /* Size of info header */
  int biWidth;                                   /* Width of image */
  int biHeight;                                  /* Height of image */
  unsigned short biPlanes = 1;                   /* Number of color planes */
  unsigned short biBitCount = 24;                /* Number of bits per pixel */
  unsigned int biCompression = 0;  /* Type of compression to use */
  unsigned int biSizeImage = 0;    /* Size of image data */
  int biXPelsPerMeter = 5000;      /* X pixels per meter */
  int biYPelsPerMeter = 5000;      /* Y pixels per meter */
  unsigned int biClrUsed = 0;      /* Number of colors used */
  unsigned int biClrImportant = 0; /* Number of important colors */
};

void WriteBMP(int width, int height, std::span<std::uint8_t> img,
              const std::string &filename) {

  if (width % 4) {
    std::cerr << "width is not a multiple of 4" << std::endl;
    exit(-1);
  }

  std::ofstream bmp(filename, std::ios::out);

  BMP_FILE_HEADER fileh;
  BMP_INFO_HEADER infoh;

  unsigned short bfType = 0x4D42;
  fileh.bfSize = 2 + sizeof(fileh) + sizeof(infoh) + width * height * 3;

  infoh.biWidth = width;
  infoh.biHeight = height;

  bmp.write(reinterpret_cast<char *>(&bfType), sizeof(bfType));
  bmp.write(reinterpret_cast<char *>(&fileh), sizeof(fileh));
  bmp.write(reinterpret_cast<char *>(&infoh), sizeof(infoh));
  bmp.write(reinterpret_cast<char *>(img.data()), img.size());
}
auto write_color_bmp(color pixel_color, std::array<std::uint8_t, 3> &now)
    -> void {
  now = {static_cast<unsigned char>(256 * pixel_color.z),
         static_cast<unsigned char>(256 * pixel_color.y),
         static_cast<unsigned char>(256 * pixel_color.x)};
}

void write_color_bmp(color pixel_color, std::array<std::uint8_t, 3> &now,
                     int samples_per_pixel) {
  // Divide the color by the number of samples.
  auto scale = 1.0 / samples_per_pixel;
  pixel_color *= scale;
  // gamma Correction to accurate color intensity
  sqrt(pixel_color);
  // Write the translated [0,255] value of each color component.
  now = {static_cast<unsigned char>(256 * clamp(pixel_color.z, 0.0, 0.999)),
         static_cast<unsigned char>(256 * clamp(pixel_color.y, 0.0, 0.999)),
         static_cast<unsigned char>(256 * clamp(pixel_color.x, 0.0, 0.999))};
}
} // namespace bmp

#endif