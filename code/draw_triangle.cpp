/*
  Created by sy 2023/5/31
*/
#include "draw.hpp"
#include "matrix.hpp"
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>

auto draw::graphics::draw_two_triangle(int argc, const char **argv) -> int {
  int width = 700, height = 700;
  float angle = 0;
  bool command_line = false;
  std::string filename = "output.png";

  if (argc == 2) {
    command_line = true;
    filename = std::string(argv[1]);
  }

  rst::rasterizer r(width, height);

  Eigen::Vector3f eye_pos = {0, 0, 5};

  std::vector<Eigen::Vector3f> pos{{2, 0, -2},     {0, 2, -2},
                                   {-2, 0, -2},    {3.5, -1, -5},
                                   {2.5, 1.5, -5}, {-1, 0.5, -5}};

  std::vector<Eigen::Vector3i> ind{{0, 1, 2}, {3, 4, 5}};

  std::vector<Eigen::Vector3f> cols{
      {217.0, 238.0, 185.0}, {217.0, 238.0, 185.0}, {217.0, 238.0, 185.0},
      {185.0, 217.0, 238.0}, {185.0, 217.0, 238.0}, {185.0, 217.0, 238.0}};

  auto pos_id = r.load_positions(pos);
  auto ind_id = r.load_indices(ind);
  auto col_id = r.load_colors(cols);

  int key = 0;
  int frame_count = 0;

  if (command_line) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);

    r.set_model(matrix::mat::get_model_matrix(angle));
    r.set_view(matrix::mat::get_view_matrix(eye_pos));
    r.set_projection(matrix::mat::get_projection_matrix(45, 1, 0.1, 50));

    r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
    cv::Mat image(height, width, CV_32FC3, r.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

    cv::imwrite(filename, image);

    return 0;
  }

  while (key != 27) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);

    r.set_model(matrix::mat::get_model_matrix(angle));
    r.set_view(matrix::mat::get_view_matrix(eye_pos));
    r.set_projection(matrix::mat::get_projection_matrix(45, 1, 0.1, 50));

    r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

    cv::Mat image(height, width, CV_32FC3, r.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    cv::imshow("image", image);
    key = cv::waitKey(10);

    std::cout << "frame count: " << frame_count++ << '\n';
  }
  return 0;
}
