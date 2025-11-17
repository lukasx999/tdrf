#include <cmath>
#include <print>
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
    //     {w/2, 0, 0, 0},
    //     {w, h, 0, 0},
    //     {0, h, 0, 0},
    //     Color::blue()
    // );
    //
    // ras.draw_triangle(
    //     {0, 0, z, 0},
    //     {w, 0, z, 0},
    //     {w/2, h, z, 0},
    //     Color::red()
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

    rl::InitWindow(width, height, "ras");

    while (!rl::WindowShouldClose()) {
        rl::BeginDrawing();
        rl::ClearBackground(rl::BLACK);

        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                rl::Color c = std::bit_cast<rl::Color>(color_buffer.get(x, y));
                rl::DrawRectangle(x, y, 1, 1, c);
            }
        }

        rl::EndDrawing();
    }

    rl::CloseWindow();

}
