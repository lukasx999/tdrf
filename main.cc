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

namespace {

void write_to_ppm(const char* filename, const Rasterizer& ras) {
    std::ofstream file(filename);

    file << "P6" << ' ';
    file << ras.get_width() << ' ';
    file << ras.get_height() << ' ';
    file << 0xff << '\n';

    for (int y = 0; y < ras.get_height(); ++y) {
        for (int x = 0; x < ras.get_width(); ++x) {
            auto color = ras.get_pixel(x, y);
            file.write(reinterpret_cast<const char*>(&color.r), 1);
            file.write(reinterpret_cast<const char*>(&color.g), 1);
            file.write(reinterpret_cast<const char*>(&color.b), 1);
        }
    }

}

[[nodiscard]] std::vector<Vec> load_obj(const char* filename) {
    std::ifstream file(filename);

    std::vector<Vec> vertices;
    std::vector<Vec> vertices_final;

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

                vertices_final.push_back(vertices[value1-1]);
                vertices_final.push_back(vertices[value2-1]);
                vertices_final.push_back(vertices[value3-1]);

            } break;
        }
    }

    return vertices_final;
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

void rl_draw_color_buffer(const Rasterizer& ras) {

    for (int y = 0; y < ras.get_height(); ++y) {
        for (int x = 0; x < ras.get_width(); ++x) {
            rl::Color c = std::bit_cast<rl::Color>(ras.get_pixel(x, y));

            // float d = (depth_buffer.get(x, y) + 1) / 2;
            // rl::Color c(d*0xff, d*0xff, d*0xff, 0xff);

            int width = rl::GetScreenWidth() / ras.get_width();
            int height = rl::GetScreenHeight() / ras.get_height();
            rl::DrawRectangle(x*width, y*height, width, height, c);
        }
    }
}

void demo_obj(Rasterizer& ras, const char* filename) {

    auto obj_vertices = load_obj(filename);

    auto vs = [](Vec p) {
        float s = 0.2;
        auto scale = Mat::scale({s, s, s, 1});
        auto angle = fmodf((rl::GetTime() * 30), 360);
        auto rot = Mat::rotate(Vec {1.0f, 1.0f, 0.0f, 1.0f}, deg_to_rad(angle));
        return rot * scale * p;
    };

    auto fs = [](Vec) {
        return Color::blue();
    };

    ras.render_vertex_buffer(obj_vertices, vs, fs);

}

void demo_triangle(Rasterizer& ras) {

    std::array vertices {
        Vec(0, 0, 0, 1),
        Vec(0.9, 0, 0, 1),
        Vec(0, 0.9, 0, 1),
    };

    auto vs = [](Vec p) {
        return p;
    };

    auto fs = [](Vec) {
        return Color::white();
    };

    ras.render_vertex_buffer(vertices, vs, fs);
}

} // namespace

int main() {

    test();

    int w = 100;
    int h = 100;
    Rasterizer ras(w, h);

    std::array cube_vertices {

        // back
        Vec(-0.5f, -0.5f, -0.5f, 1.0f),
        Vec( 0.5f, -0.5f, -0.5f, 1.0f),
        Vec( 0.5f,  0.5f, -0.5f, 1.0f),
        Vec( 0.5f,  0.5f, -0.5f, 1.0f),
        Vec(-0.5f,  0.5f, -0.5f, 1.0f),
        Vec(-0.5f, -0.5f, -0.5f, 1.0f),

        // front
        Vec(-0.5f, -0.5f,  0.5f, 1.0f),
        Vec( 0.5f, -0.5f,  0.5f, 1.0f),
        Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        Vec(-0.5f,  0.5f,  0.5f, 1.0f),
        Vec(-0.5f, -0.5f,  0.5f, 1.0f),

        // left
        Vec(-0.5f,  0.5f,  0.5f, 1.0f),
        Vec(-0.5f,  0.5f, -0.5f, 1.0f),
        Vec(-0.5f, -0.5f, -0.5f, 1.0f),
        Vec(-0.5f, -0.5f, -0.5f, 1.0f),
        Vec(-0.5f, -0.5f,  0.5f, 1.0f),
        Vec(-0.5f,  0.5f,  0.5f, 1.0f),

        // right
        Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        Vec( 0.5f,  0.5f, -0.5f, 1.0f),
        Vec( 0.5f, -0.5f, -0.5f, 1.0f),
        Vec( 0.5f, -0.5f, -0.5f, 1.0f),
        Vec( 0.5f, -0.5f,  0.5f, 1.0f),
        Vec( 0.5f,  0.5f,  0.5f, 1.0f),

        // bottom
        Vec(-0.5f, -0.5f, -0.5f, 1.0f),
        Vec( 0.5f, -0.5f, -0.5f, 1.0f),
        Vec( 0.5f, -0.5f,  0.5f, 1.0f),
        Vec( 0.5f, -0.5f,  0.5f, 1.0f),
        Vec(-0.5f, -0.5f,  0.5f, 1.0f),
        Vec(-0.5f, -0.5f, -0.5f, 1.0f),

        // top
        Vec(-0.5f,  0.5f, -0.5f, 1.0f),
        Vec( 0.5f,  0.5f, -0.5f, 1.0f),
        Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        Vec(-0.5f,  0.5f,  0.5f, 1.0f),
        Vec(-0.5f,  0.5f, -0.5f, 1.0f),

    };

    write_to_ppm("out.ppm", ras);

    rl::SetConfigFlags(rl::FLAG_WINDOW_RESIZABLE);
    rl::InitWindow(1600, 900, "ras");

    while (!rl::WindowShouldClose()) {
        rl::BeginDrawing();
        rl::ClearBackground(rl::BLACK);

        ras.clear();

        // TODO: projection matrix
        // demo_obj(ras, "teapot.obj");
        demo_triangle(ras);

        rl_draw_color_buffer(ras);

        rl::DrawFPS(0, 0);

        rl::EndDrawing();
    }

    rl::CloseWindow();

}
