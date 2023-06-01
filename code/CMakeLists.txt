cmake_minimum_required(VERSION 3.10)
project(Rasterizer)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/local/include)

add_executable(Rasterizer main.cpp OBJ_Loader.hpp Triangle.hpp Triangle.cpp Texture.hpp Texture.cpp Shader.hpp Shader.cpp  rasterizer.hpp rasterizer.cpp matrix.hpp matrix.cpp draw.hpp draw_triangle.cpp)
target_link_libraries(Rasterizer ${OpenCV_LIBRARIES})
