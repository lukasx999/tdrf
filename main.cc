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

// TODO: wavefront obj loader

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

int main() {

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

    float h = height;
    float w = width;
    float z = -1.0;

    // ras.draw_triangle(
    //     {w/2, 0, -50, 0},
    //     {w, h, -100, 0},
    //     {0, h, 0, 0},
    //     Color::blue()
    // );

    for (auto& face : cube_copy.faces) {
        for (auto& t : face.triangles) {
            auto rot_mat = Mat::rotate(Vec {1.0f, 1.0f, 0.0f, 1.0f}, deg_to_rad(45));
            t.a = rot_mat * t.a;
            t.b = rot_mat * t.b;
            t.c = rot_mat * t.c;
        }
    }

    for (auto&& [face_nr, face] : cube_copy.faces | std::views::enumerate) {
        for (auto& t : face.triangles) {
            auto color = face_color_map[face_nr];
            float w = 100;
            float h = 100;
            float d = 100;
            Vec offset {width/2.0f-w/2.0f, height/2.0f-h/2.0f};
            ras.draw_triangle(t.a*w+offset, t.b*h+offset, t.c*d+offset, color);
        }
    }

    write_to_ppm("out.ppm", color_buffer);

    rl::InitWindow(width, height, "ras");

    while (!rl::WindowShouldClose()) {
        rl::BeginDrawing();
        rl::ClearBackground(rl::BLACK);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                rl::Color c = std::bit_cast<rl::Color>(color_buffer.get(x, y));

                // float d = 1-(depth_buffer.get(x, y) / -100);
                // rl::Color c(d*0xff, d*0xff, d*0xff, 0xff);

                rl::DrawRectangle(x, y, 1, 1, c);
            }
        }

        rl::EndDrawing();
    }

    rl::CloseWindow();

}
