#include <cmath>
#include <print>
#include <fstream>
#include <vector>
#include <ranges>
#include <array>

namespace rl {
#include <raylib.h>
}

#include "math.h"
#include "ras.h"
#include "aux.h"

namespace {

void write_to_ppm(const char* filename, const ColorBuffer& color_buffer) {
    std::ofstream file(filename);

    file << "P6" << ' ';
    file << color_buffer.get_width() << ' ';
    file << color_buffer.get_height() << ' ';
    file << 0xff << '\n';

    for (int y = 0; y < color_buffer.get_height(); ++y) {
        for (int x = 0; x < color_buffer.get_width(); ++x) {
            auto color = color_buffer.get(x, y);
            file.write(reinterpret_cast<const char*>(&color.r), 1);
            file.write(reinterpret_cast<const char*>(&color.g), 1);
            file.write(reinterpret_cast<const char*>(&color.b), 1);
        }
    }

}

[[nodiscard]] std::vector<Triangle> load_obj(const char* filename) {
    std::ifstream file(filename);

    std::vector<Vec> vertices;
    std::vector<Triangle> faces;

    std::string line;
    while (std::getline(file, line)) {
        switch (line[0]) {
            case 'v': {
                size_t offset = 2;

                size_t len = line.find(' ', offset) - offset;
                std::string num1 = line.substr(offset, len);
                offset += len+1;

                len = line.find(' ', offset) - offset;
                std::string num2 = line.substr(offset, len);
                offset += len+1;

                len = line.find(' ', offset) - offset;
                std::string num3 = line.substr(offset, len);

                float value1;
                std::from_chars(num1.c_str(), num1.c_str()+num1.size(), value1);
                float value2;
                std::from_chars(num2.c_str(), num2.c_str()+num2.size(), value2);
                float value3;
                std::from_chars(num3.c_str(), num3.c_str()+num3.size(), value3);

                vertices.push_back({value1, value2, value3, 1.0f});

                // std::println("[{}, {}, {}] => {}, {}, {}", num1, num2, num3, value1, value2, value3);

            } break;

            case 'f': {

                size_t offset = 2;

                size_t len = line.find(' ', offset) - offset;
                std::string num1 = line.substr(offset, len);
                offset += len+1;

                len = line.find(' ', offset) - offset;
                std::string num2 = line.substr(offset, len);
                offset += len+1;

                len = line.find(' ', offset) - offset;
                std::string num3 = line.substr(offset, len);

                int value1;
                std::from_chars(num1.c_str(), num1.c_str()+num1.size(), value1);
                int value2;
                std::from_chars(num2.c_str(), num2.c_str()+num2.size(), value2);
                int value3;
                std::from_chars(num3.c_str(), num3.c_str()+num3.size(), value3);

                faces.push_back({ vertices[value1-1], vertices[value2-1], vertices[value3-1] });

            } break;
        }
    }

    return faces;
}

void test_vector_matrix() {

    Vec v(2, 6, 1, 1);
    Mat m1({
        Vec(10, 0, 66, 1),
        Vec(2, 17, 3, 1),
        Vec(1, 0, 4, 24),
        Vec(1, 1, 9, 1),
    });

    auto r = m1 * v;
    assert(r.x == 34);
    assert(r.y == 103);
    assert(r.z == 163);
    assert(r.w == 33);
}

void test_translate() {

    Mat translate = Mat::translate({2, 2, 7, 1});
    auto r = translate * Vec(3, 2, 3, 1);
    assert(r.x == 5);
    assert(r.y == 4);
    assert(r.z == 10);
    assert(r.w == 1);
}

void test_scale() {

    Mat scale = Mat::scale({2, 2, 2, 2});
    auto r = scale * Vec(1, 2, 3, 4);
    assert(r.x == 2);
    assert(r.y == 4);
    assert(r.z == 6);
    assert(r.w == 4);
}

void test_rotate() {
    float eps = 1e-3;
    Mat rot = Mat::rotate({1, 0, 0, 1}, deg_to_rad(90));
    auto r = rot * Vec(0, 0, 1, 1);
    assert(r.x == 0);
    assert(r.y == -1);
    assert(r.z < eps);
}

void test() {

    test_vector_matrix();
    test_translate();
    test_scale();
    test_rotate();

}

void rl_draw_color_buffer(const ColorBuffer& color_buffer) {

    for (int y = 0; y < color_buffer.get_height(); ++y) {
        for (int x = 0; x < color_buffer.get_width(); ++x) {
            rl::Color c = std::bit_cast<rl::Color>(color_buffer.get(x, y));

            // float d = 1-(depth_buffer.get(x, y) / -150);
            // rl::Color c(d*0xff, d*0xff, d*0xff, 0xff);

            rl::DrawRectangle(x, y, 1, 1, c);
        }
    }
}

} // namespace

int main() {

    test();

    ColorBuffer color_buffer(1600, 900);
    DepthBuffer depth_buffer(1600, 900);
    Rasterizer ras(color_buffer, depth_buffer);
    int width = color_buffer.get_width();
    int height = color_buffer.get_height();

    std::array<Color, 6> face_color_map {
        Color(0xff, 0x0, 0x0, 0xff),
        Color(0x0, 0xff, 0x0, 0xff),
        Color(0x0, 0x0, 0xff, 0xff),
        Color(0x7f, 0x0, 0x0, 0xff),
        Color(0x0, 0x7f, 0x0, 0xff),
        Color(0x0, 0x0, 0x7f, 0xff),
    };

    Cube cube_copy(cube);

    // TODO: convert to opengl ndc

    auto teapot_triangles = load_obj("teapot.obj");
    auto teapot_triangles_clone(teapot_triangles);

    Vec t1(0.5, 0, 0, 1);
    Vec t2(1, 1, 0, 1);
    Vec t3(0, 1, 0, 1);

    // for (auto& face : cube_copy.faces) {
    //     for (auto& t : face.triangles) {
    //         auto rot_mat = Mat::rotate(Vec {1.0f, 1.0f, 0.0f, 1.0f}, deg_to_rad(45));
    //         t.a = rot_mat * t.a;
    //         t.b = rot_mat * t.b;
    //         t.c = rot_mat * t.c;
    //     }
    // }
    //
    // for (auto&& [face_nr, face] : cube_copy.faces | std::views::enumerate) {
    //     for (auto& t : face.triangles) {
    //         auto color = face_color_map[face_nr];
    //         float w = 100;
    //         float h = 100;
    //         float d = 100;
    //         Vec offset {width/2.0f-w/2.0f, height/2.0f-h/2.0f};
    //         ras.draw_triangle(t.a*w+offset, t.b*h+offset, t.c*d+offset, color);
    //     }
    // }


    // write_to_ppm("out.ppm", color_buffer);

    rl::InitWindow(width, height, "ras");

    while (!rl::WindowShouldClose()) {
        rl::BeginDrawing();
        rl::ClearBackground(rl::BLACK);

        ras.clear();

        // for (auto&& [t, t_clone] : std::views::zip(teapot_triangles, teapot_triangles_clone)) {
        //     float scale = 30;
        //
        //     // TODO: fix matrix translation
        //     auto rot_mat = Mat::rotate({1.0f, 0.0f, 0.0f, 1.0f}, deg_to_rad(rl::GetTime()*50));
        //     auto scale_mat = Mat::scale({scale, scale, scale, 1.0f});
        //     auto transf_mat = Mat::translate({width/2.0f, height/2.0f, 0.0f, 1.0f});
        //     auto mat = transf_mat * scale_mat * rot_mat;
        //     t.a = mat * t_clone.a;
        //     t.b = mat * t_clone.b;
        //     t.c = mat * t_clone.c;
        //
        //     ras.draw_triangle(t.a, t.b, t.c, Color::blue());
        // }

        rl_draw_color_buffer(color_buffer);


        rl::EndDrawing();
    }

    rl::CloseWindow();

}
