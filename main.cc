#include <cmath>
#include <print>
#include <vector>
#include <ranges>
#include <array>

namespace rl {
#include <raylib.h>
}

#include "math.h"

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0xff;
};

static_assert(sizeof(Color) == 4);

class ColorBuffer {
    static constexpr int m_width = 900;
    static constexpr int m_height = 900;
    std::vector<Color> m_color_buffer{ m_width * m_height };

public:
    ColorBuffer() = default;

    void write(int x, int y, Color color) {
        m_color_buffer[y * m_width + x] = color;
    }

    [[nodiscard]] Color get(int x, int y) const {
        return m_color_buffer[y * m_width + x];
    }

    [[nodiscard]] int get_width() const {
        return m_width;
    }

    [[nodiscard]] int get_height() const {
        return m_height;
    }

};

class Rasterizer {
    ColorBuffer& m_color_buffer;

public:
    explicit Rasterizer(ColorBuffer& color_buffer) : m_color_buffer(color_buffer) { }

    void draw_triangle(Vec a, Vec b, Vec c, Color color) {

        // TODO: only check bounding box of triangle
        for (int x = 0; x < m_color_buffer.get_width(); ++x) {
            for (int y = 0; y < m_color_buffer.get_height(); ++y) {
                Vec p { static_cast<float>(x), static_cast<float>(y), 0.0f, 1.0f };

                int abp = edge_function(a, b, p);
                int bcp = edge_function(b, c, p);
                int cap = edge_function(c, a, p);

                bool wireframe = false;
                if (wireframe) {
                    // TODO: thicker wireframe lines
                    if (abp == 0 && bcp == 0 && cap == 0) {
                        m_color_buffer.write(x, y, color);
                    }

                } else {
                    if (abp > 0 && bcp > 0 && cap > 0) {
                        m_color_buffer.write(x, y, color);
                    }
                }
            }
        }

    }

    void clear(Color color) {
        for (int x = 0; x < m_color_buffer.get_width(); ++x) {
            for (int y = 0; y < m_color_buffer.get_height(); ++y) {
                m_color_buffer.write(x, y, color);
            }
        }
    }

private:
    // returns the area of a triangle, which may be negative
    [[nodiscard]] static constexpr float edge_function(Vec a, Vec b, Vec c) {
        return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
    }

};

struct Triangle {
    Vec a;
    Vec b;
    Vec c;
};

struct Face {
    std::array<Triangle, 2> triangles;
};

struct Cube {
    std::array<Face, 6> faces;
};

int main() {

    Cube cube {
        Face {
            Triangle { Vec(0.0f, 0.0f, 0.0f), Vec(1.0f, 0.0f, 0.0f), Vec(1.0f, 1.0f, 0.0f) },
            Triangle { Vec(1.0f, 1.0f, 0.0f), Vec(0.0f, 1.0f, 0.0f), Vec(0.0f, 0.0f, 0.0f) },
        },
        Face {
            Triangle { Vec(0.0f, 0.0f, 1.0f), Vec(1.0f, 0.0f, 1.0f), Vec(1.0f, 1.0f, 1.0f) },
            Triangle { Vec(1.0f, 1.0f, 1.0f), Vec(0.0f, 1.0f, 1.0f), Vec(0.0f, 0.0f, 1.0f) },
        },
        Face {
            Triangle { Vec(0.0f, 1.0f, 1.0f), Vec(0.0f, 1.0f, 0.0f), Vec(0.0f, 0.0f, 0.0f) },
            Triangle { Vec(0.0f, 0.0f, 0.0f), Vec(0.0f, 0.0f, 1.0f), Vec(0.0f, 1.0f, 1.0f) },
        },
        Face {
            Triangle { Vec(1.0f, 1.0f, 1.0f), Vec(1.0f, 1.0f, 0.0f), Vec(1.0f, 0.0f, 0.0f) },
            Triangle { Vec(1.0f, 0.0f, 0.0f), Vec(1.0f, 0.0f, 1.0f), Vec(1.0f, 1.0f, 1.0f) },
        },
        Face {
            Triangle { Vec(0.0f, 0.0f, 0.0f), Vec(1.0f, 0.0f, 0.0f), Vec(1.0f, 0.0f, 1.0f) },
            Triangle { Vec(1.0f, 0.0f, 1.0f), Vec(0.0f, 0.0f, 1.0f), Vec(0.0f, 0.0f, 0.0f) },
        },
        Face {
            Triangle { Vec(0.0f, 1.0f, 0.0f), Vec(1.0f, 1.0f, 0.0f), Vec(1.0f, 1.0f, 1.0f) },
            Triangle { Vec(1.0f, 1.0f, 1.0f), Vec(0.0f, 1.0f, 1.0f), Vec(0.0f, 1.0f, 0.0f) },
        },
    };


    ColorBuffer color_buffer;
    Rasterizer ras(color_buffer);
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

    for (auto& face : cube.faces) {
        for (auto& t : face.triangles) {
            auto rot_mat = Mat::rotate(Vec {1.0f, 1.0f, 0.0f, 1.0f}, deg_to_rad(25));
            t.a = rot_mat * t.a;
            t.b = rot_mat * t.b;
            t.c = rot_mat * t.c;
        }
    }

    for (auto&& [face_nr, face] : cube.faces | std::views::enumerate) {
        for (auto& t : face.triangles) {
            auto color = face_color_map[face_nr];
            float w = 200;
            float h = 200;
            float d = 200;
            Vec offset {width/2.0f-w/2.0f, height/2.0f-h/2.0f};
            ras.draw_triangle(t.a*w+offset, t.b*h+offset, t.c*d+offset, color);
        }
    }

    // ras.draw_triangle({0, 0, 0}, {width, 0, 0}, {width/2, height, 0}, Color(0xff, 0x0, 0x0, 0xff));

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
