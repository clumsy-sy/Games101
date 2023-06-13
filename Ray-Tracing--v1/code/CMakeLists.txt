cmake_minimum_required(VERSION 3.10)
project(RayTracing)
set(CMAKE_CXX_STANDARD 20)

aux_source_directory(./ HPP)
add_executable(RayTracing ${HPP})

target_compile_options(RayTracing PUBLIC -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
target_compile_features(RayTracing PUBLIC cxx_std_20)
target_link_libraries(RayTracing PUBLIC -fsanitize=undefined)
