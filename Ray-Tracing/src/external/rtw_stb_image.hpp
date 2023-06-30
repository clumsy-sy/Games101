#ifndef RTWEEKEND_STB_IMAGE_H
#define RTWEEKEND_STB_IMAGE_H

// 不显示 warming

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define STB_IMAGE_IMPLEMENTATION
// auto stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) -> unsigned char *;
#include "stb_image.hpp"

#pragma GCC diagnostic pop

#endif