#include "Shader.hpp"

auto vertex_shader(const vertex_shader_payload &payload) -> Eigen::Vector3f {
    return payload.position;
}

template <class T1, class T2>
auto qpow(T1 a, T2 b) -> T1 {
    if constexpr (std::is_integral_v<T2>) {
        T1 ret = 1.0;
        while (b) {
            if (b & 1)
                ret = ret * a;
            a = a * a;
            b >>= 1;
        }
        return ret;
    } else {
        return std::pow(a, b);
    }
}

auto normal_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f {
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() +
                                    Eigen::Vector3f(1.0f, 1.0f, 1.0f)) /
                                   2.f;
    auto result = Eigen::Vector3f(return_color);
    result << return_color.x() * 255, return_color.y() * 255,
            return_color.z() * 255;
    return result;
}

static auto reflect(const Eigen::Vector3f &vec, const Eigen::Vector3f &axis)
-> Eigen::Vector3f {
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

auto phong_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f {
    /*
      k_a: ambient coefficient 环境光追系数
      k_d: diffuse coefficient 扩散系数
      k_s: specular coefficient 镜面反射系数
    */
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);
    // 两个光源
    auto l1 = light{{20,  20,  20},
                    {500, 500, 500}};
    auto l2 = light{{-20, 20,  0},
                    {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

//  float p = 150; // 幂次系数
    int p = 150;
    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto &light: lights) {
        // TODO: For each light source in the code, calculate what the *ambient*,
        // *diffuse*, and *specular* components are. Then, accumulate that result on
        // the *result_color* object.
        /*
          对于所有的光源，都要计算对应的 ambient（环境光照）diffuse（漫反射）
          specular（高光）
        */
        /*
          r 为模长（光到物体表面点的距离）
          l：入射法向量
          n：照射点的法向量
          v：点到观测点的法向量
        */
        auto l2p = light.position - point;
        float r_square = l2p.dot(l2p); // r^2
        auto l = l2p.normalized();
        auto n = normal.normalized();
        auto v = (eye_pos - point).normalized();
        // L_d = k_d (I / r^2 )max(0, n \cdot l)
        auto ambient = ka.cwiseProduct(amb_light_intensity);
        auto diffuse = kd.cwiseProduct(light.intensity / r_square) *
                       std::max(0.0, (double) n.dot(l));
        auto h = (v + l).normalized(); // 半程向量
        auto specular = ks.cwiseProduct(light.intensity / r_square) *
                        qpow(std::max(0.0, (double) n.dot(h)), p);
        /*
          L = L_a + L_d + L_s
            = k_a * L_a + k_d (I / r^2)max(0, n dot l) + k_s(I / r^2)max(0, n dot
          h)^p
        */
        result_color += ambient + diffuse + specular;
    }

    return result_color * 255.f;
}

auto texture_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f {
    Eigen::Vector3f texture_color = {0, 0, 0};
    if (payload.texture) {
        // TODO: Get the texture value at the texture coordinates of the current
        // fragment
        texture_color = payload.texture->getColor(payload.tex_coords.x(),
                                                  payload.tex_coords.y());
    }
    // auto texture_color = Vector3f(return_color);

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20,  20,  20},
                    {500, 500, 500}};
    auto l2 = light{{-20, 20,  0},
                    {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto &light: lights) {
        float r = (light.position - point).norm();
        auto l = (light.position - point).normalized();
        auto n = normal.normalized();
        auto v = (eye_pos - point).normalized();
        auto ambient = ka.cwiseProduct(amb_light_intensity);
        auto diffuse = kd.cwiseProduct(light.intensity / (r * r)) *
                       std::max(0.0, (double) n.dot(l));
        auto h = (v + l).normalized(); // 半程向量
        auto specular = ks.cwiseProduct(light.intensity / (r * r)) *
                        pow(std::max(0.0, (double) n.dot(h)), p);

        result_color += ambient + diffuse + specular;
    }
    return result_color * 255.f;
}

auto displacement_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f {

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20,  20,  20},
                    {500, 500, 500}};
    auto l2 = light{{-20, 20,  0},
                    {500, 500, 500}};

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
    auto &w = payload.texture->width, &h = payload.texture->height;
    auto &u = payload.tex_coords.x(), &v = payload.tex_coords.y();
    auto &n = normal.normalized();
    double x = n.x(), y = n.y(), z = n.z();
    auto t = Eigen::Vector3f(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z),
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
    auto ln = Eigen::Vector3f(-dU, -dV, 1.0);
    point = point + kn * normal * payload.texture->getColor(u, v).norm();

    normal = (TBN * ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto &light: lights) {
        float r = (light.position - point).norm();
        auto l = (light.position - point).normalized();
        auto n = normal.normalized();
        auto v = (eye_pos - point).normalized();

        auto ambient = ka.cwiseProduct(amb_light_intensity);
        auto diffuse = kd.cwiseProduct(light.intensity / (r * r)) *
                       std::max(0.0, (double) n.dot(l));
        auto h = (v + l).normalized();
        auto specular = ks.cwiseProduct(light.intensity / (r * r)) *
                        pow(std::max(0.0, (double) h.dot(n)), p);

        result_color += ambient + diffuse + specular;
    }

    return result_color * 255.f;
}

auto bump_fragment_shader(const fragment_shader_payload &payload)
-> Eigen::Vector3f {

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20,  20,  20},
                    {500, 500, 500}};
    auto l2 = light{{-20, 20,  0},
                    {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

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
    auto t = Eigen::Vector3f(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z),
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
    auto ln = Eigen::Vector3f(-dU, -dV, 1.0);
    result_color = (TBN * ln).normalized();

    return result_color * 255.f;
}