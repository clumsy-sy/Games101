

#include "OBJ_Loader.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Triangle.hpp"
#include "matrix.hpp"
#include "rasterizer.hpp"
#include <system_error>

auto vertex_shader(const vertex_shader_payload &payload) -> Eigen::Vector3f {
  return payload.position;
}

auto normal_fragment_shader(const fragment_shader_payload &payload)
    -> Eigen::Vector3f {
  Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() +
                                  Eigen::Vector3f(1.0f, 1.0f, 1.0f)) /
                                 2.f;
  Eigen::Vector3f result;
  result << return_color.x() * 255, return_color.y() * 255,
      return_color.z() * 255;
  return result;
}

static auto reflect(const Eigen::Vector3f &vec, const Eigen::Vector3f &axis)
    -> Eigen::Vector3f {
  auto costheta = vec.dot(axis);
  return (2 * costheta * axis - vec).normalized();
}

struct light {
  Eigen::Vector3f position;
  Eigen::Vector3f intensity;
};

auto texture_fragment_shader(const fragment_shader_payload &payload)
    -> Eigen::Vector3f {
  Eigen::Vector3f return_color = {0, 0, 0};
  if (payload.texture) {
    // TODO: Get the texture value at the texture coordinates of the current
    // fragment
    return_color = payload.texture->getColor(payload.tex_coords.x(),
                                             payload.tex_coords.y());
  }
  auto texture_color = Vector3f(return_color);
  /*
    k_a: ambient coefficient 环境光追系数
    k_d: diffuse coefficient 扩散系数
    k_s: specular coefficient 镜面反射系数
  */
  Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f kd = texture_color / 255.f;
  Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);
  // 两个光源
  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  float p = 150; // 幂次系数

  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  Eigen::Vector3f result_color = {0, 0, 0};
  /*
    对于所有的光源，都要计算对应的 ambient（环境光照）diffuse（漫反射） specular（高光）
  */
  for (auto &light : lights) {
    // TODO: For each light source in the code, calculate what the *ambient*,
    // *diffuse*, and *specular* components are. Then, accumulate that result on
    // the *result_color* object.
    float r = (light.position - point).norm();
    auto l = (light.position - point).normalized();
    auto n = normal.normalized();
    auto v = (eye_pos - point).normalized();

    auto ambient = ka.cwiseProduct(amb_light_intensity);
    auto diffuse = kd.cwiseProduct(light.intensity / (r * r)) *
                   std::max(0.0, (double)n.dot(l));
    auto h = (v + l).normalized(); // 半程向量
    auto specular = ks.cwiseProduct(light.intensity / (r * r)) *
                    pow(std::max(0.0, (double)h.dot(n)), p);
    /*
      L = L_a + L_d + L_s
        = k_a * L_a + k_d (I / r^2)max(0, n dot l) + k_s(I / r^2)max(0, n dot h)
    */
    result_color += ambient + diffuse + specular;
  }

  return result_color * 255.f;
}

auto phong_fragment_shader(const fragment_shader_payload &payload)
    -> Eigen::Vector3f {
  Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f kd = payload.color;
  Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  float p = 150;

  Eigen::Vector3f color = payload.color;
  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  Eigen::Vector3f result_color = {0, 0, 0};
  for (auto &light : lights) {
    float r = (light.position - point).norm();
    auto l = (light.position - point).normalized();
    auto n = normal.normalized();
    auto v = (eye_pos - point).normalized();
    // L_d = k_d (I / r^2 )max(0, n \cdot l)
    auto ambient = ka.cwiseProduct(amb_light_intensity);
    auto diffuse = kd.cwiseProduct(light.intensity / (r * r)) *
                   std::max(0.0, (double)n.dot(l));
    auto h = (v + l).normalized();
    auto specular = ks.cwiseProduct(light.intensity / (r * r)) *
                    pow(std::max(0.0, (double)h.dot(n)), p);

    result_color += ambient + diffuse + specular;
  }

  return result_color * 255.f;
}

auto displacement_fragment_shader(const fragment_shader_payload &payload)
    -> Eigen::Vector3f {

  Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f kd = payload.color;
  Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  float p = 150;

  Eigen::Vector3f color = payload.color;
  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  float kh = 0.2, kn = 0.1;

  // TODO: Implement displacement mapping here
  // Let n = normal = (x, y, z)
  // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
  // Vector b = n cross product t
  // Matrix TBN = [t b n]
  // dU = kh * kn * (h(u+1/w,v)-h(u,v))
  // dV = kh * kn * (h(u,v+1/h)-h(u,v))
  // Vector ln = (-dU, -dV, 1)
  // Position p = p + kn * n * h(u,v)
  // Normal n = normalize(TBN * ln)
  auto w = payload.texture->width, h = payload.texture->height;
  auto u = payload.tex_coords.x(), v = payload.tex_coords.y();
  auto n = normal.normalized();
  double x = n.x(), y = n.y(), z = n.z();
  auto t = Vector3f(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z),
                    z * y / sqrt(x * x + z * z));
  auto b = n.cross(t);
  Eigen::Matrix3f TBN;
  // clang-format off
  TBN << t.x(), b.x(), n.x(),
         t.y(), b.y(), n.y(),
         t.z(), b.z(), n.z();
  // clang-format on
  auto dU = kh * kn *
            (payload.texture->getColor(std::min(u + 1.0 / w, 1.0), v).norm() -
             payload.texture->getColor(u, v).norm());
  auto dV = kh * kn *
            (payload.texture->getColor(u, std::min(v + 1.0 / h, 1.0)).norm() -
             payload.texture->getColor(u, v).norm());
  point = point + kn * normal * payload.texture->getColor(u, v).norm();
  normal = (TBN * Vector3f(-dU, -dV, 1.0)).normalized();

  Eigen::Vector3f result_color = {0, 0, 0};

  for (auto &light : lights) {
    float r = (light.position - point).norm();
    auto l = (light.position - point).normalized();
    auto n = normal.normalized();
    auto v = (eye_pos - point).normalized();

    auto ambient = ka.cwiseProduct(amb_light_intensity);
    auto diffuse = kd.cwiseProduct(light.intensity / (r * r)) *
                   std::max(0.0, (double)n.dot(l));
    auto h = (v + l).normalized();
    auto specular = ks.cwiseProduct(light.intensity / (r * r)) *
                    pow(std::max(0.0, (double)h.dot(n)), p);

    result_color += ambient + diffuse + specular;
  }

  return result_color * 255.f;
}

auto bump_fragment_shader(const fragment_shader_payload &payload)
    -> Eigen::Vector3f {

  Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f kd = payload.color;
  Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  float p = 150;

  Eigen::Vector3f color = payload.color;
  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  float kh = 0.2, kn = 0.1;

  // TODO: Implement bump mapping here
  // Let n = normal = (x, y, z)
  // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
  // Vector b = n cross product t
  // Matrix TBN = [t b n]
  // dU = kh * kn * (h(u+1/w,v)-h(u,v))
  // dV = kh * kn * (h(u,v+1/h)-h(u,v))
  // Vector ln = (-dU, -dV, 1)
  // Normal n = normalize(TBN * ln)

  Eigen::Vector3f result_color = {0, 0, 0};
  result_color = normal;
  auto w = payload.texture->width, h = payload.texture->height;
  auto u = payload.tex_coords.x(), v = payload.tex_coords.y();
  auto n = normal.normalized();
  double x = n.x(), y = n.y(), z = n.z();
  auto t = Vector3f(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z),
                    z * y / sqrt(x * x + z * z));
  auto b = n.cross(t);
  Eigen::Matrix3f TBN;
  // clang-format off
  TBN << t.x(), b.x(), n.x(), 
         t.y(), b.y(), n.y(), 
         t.z(), b.z(), n.z();
  // clang-format on
  auto dU = kh * kn *
            (payload.texture->getColor(std::min(u + 1.0 / w, 1.0), v).norm() -
             payload.texture->getColor(u, v).norm());
  auto dV = kh * kn *
            (payload.texture->getColor(u, std::min(v + 1.0 / h, 1.0)).norm() -
             payload.texture->getColor(u, v).norm());
  result_color = (TBN * Vector3f(-dU, -dV, 1.0)).normalized();

  return result_color * 255.f;
}

auto main(int argc, const char **argv) -> int {
  std::vector<Triangle *> TriangleList;

  float angle = 140.0;
  bool command_line = false;

  std::string filename = "output.png";
  objl::Loader Loader;
  std::string obj_path = "../models/spot/";

  // Load .obj File
  bool loadout = Loader.LoadFile("../models/spot/spot_triangulated_good.obj");
  if(loadout == false) {
    std::cerr << "Load Error" << std::endl;
  }
  // 从 OBJ 文件中读取三角形，并且创建对应的对象
  for (auto mesh : Loader.LoadedMeshes) {
    for (int i = 0; i < (int)mesh.Vertices.size(); i += 3) {
      auto *t = new Triangle();
      for (int j = 0; j < 3; j++) {
        t->setVertex(j, Vector4f(mesh.Vertices[i + j].Position.X,
                                 mesh.Vertices[i + j].Position.Y,
                                 mesh.Vertices[i + j].Position.Z, 1.0));
        t->setNormal(j, Vector3f(mesh.Vertices[i + j].Normal.X,
                                 mesh.Vertices[i + j].Normal.Y,
                                 mesh.Vertices[i + j].Normal.Z));
        t->setTexCoord(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X,
                                   mesh.Vertices[i + j].TextureCoordinate.Y));
      }
      TriangleList.push_back(t);
    }
  }

  rst::rasterizer r(700, 700);

  auto texture_path = "hmap.jpg";
  r.set_texture(Texture(obj_path + texture_path));
  // phong 模型
  std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader =
      phong_fragment_shader;

  if (argc >= 2) {
    command_line = true;
    filename = std::string(argv[1]);

    if (argc == 3 && std::string(argv[2]) == "texture") {
      std::cout << "Rasterizing using the texture shader\n";
      active_shader = texture_fragment_shader;
      texture_path = "spot_texture.png";
      r.set_texture(Texture(obj_path + texture_path));
    } else if (argc == 3 && std::string(argv[2]) == "normal") {
      std::cout << "Rasterizing using the normal shader\n";
      active_shader = normal_fragment_shader;
    } else if (argc == 3 && std::string(argv[2]) == "phong") {
      std::cout << "Rasterizing using the phong shader\n";
      active_shader = phong_fragment_shader;
    } else if (argc == 3 && std::string(argv[2]) == "bump") {
      std::cout << "Rasterizing using the bump shader\n";
      active_shader = bump_fragment_shader;
    } else if (argc == 3 && std::string(argv[2]) == "displacement") {
      std::cout << "Rasterizing using the bump shader\n";
      active_shader = displacement_fragment_shader;
    }
  }

  Eigen::Vector3f eye_pos = {0, 0, 10};

  r.set_vertex_shader(vertex_shader);
  r.set_fragment_shader(active_shader);

  int key = 0;
  int frame_count = 0;

  if (command_line) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);
    r.set_model(matrix::mat::get_model_matrix(angle));
    r.set_view(matrix::mat::get_view_matrix(eye_pos));
    r.set_projection(matrix::mat::get_projection_matrix(45.0, 1, 0.1, 50));

    r.draw(TriangleList);
    cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

    cv::imwrite(filename, image);

    return 0;
  }

  while (key != 27) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);

    r.set_model(matrix::mat::get_model_matrix(angle));
    r.set_view(matrix::mat::get_view_matrix(eye_pos));
    r.set_projection(matrix::mat::get_projection_matrix(45.0, 1, 0.1, 50));

    // r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
    r.draw(TriangleList);
    cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

    cv::imshow("image", image);
    cv::imwrite(filename, image);
    key = cv::waitKey(10);

    if (key == 'a') {
      angle -= 0.1;
    } else if (key == 'd') {
      angle += 0.1;
    }
    std::cout << "frame count: " << frame_count++ << std::endl;
  }
  return 0;
}
