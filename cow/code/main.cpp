

#include "OBJ_Loader.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Triangle.hpp"
#include "matrix.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/src/Core/Matrix.h>
#include <system_error>
#include <type_traits>


auto main(int argc, const char **argv) -> int {
    std::vector<Triangle *> TriangleList;

    float angle = 140.0;
    bool command_line = false;

    std::string filename = "output.png";
    objl::Loader Loader;
    std::string obj_path = "../models/spot/";

    // Load .obj File
    auto loadout = Loader.LoadFile("../models/spot/spot_triangulated_good.obj");
    if (!loadout) {
        std::cerr << "Load Error" << std::endl;
    }
    // 从 OBJ 文件中读取三角形，并且创建对应的对象
    for (auto mesh: Loader.LoadedMeshes) {
        for (int i = 0; i < (int) mesh.Vertices.size(); i += 3) {
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
    std::cout << "Triangle total : " << TriangleList.size() << std::endl;

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
            std::cout << "Rasterizing using the displacement shader\n";
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

        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
//        cv::imwrite(filename, image);
        key = cv::waitKey(10);

        if (key == 'a') {
            angle -= 5;
        } else if (key == 'd') {
            angle += 5;
        } else if(key == 'c') {
            r.set_fragment_shader(normal_fragment_shader);
        } else if(key == 't') {
            r.set_fragment_shader(texture_fragment_shader);
        } else if(key == 'p') {
            r.set_fragment_shader(phong_fragment_shader);
        } else if(key == 'b') {
            r.set_fragment_shader(bump_fragment_shader);
        } else if(key == 's') {
            r.set_fragment_shader(displacement_fragment_shader);
        }
        std::cout << "frame count: " << frame_count++ << " angle: " << angle << std::endl;
    }
    return 0;
}
