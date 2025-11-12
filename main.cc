#include <cmath>
#include <print>
#include <vector>
#include <ranges>
#include <array>

namespace rl {
#include <raylib.h>
}

struct Vec {
    int x = 0;
    int y = 0;
    int z = 0;

    constexpr Vec operator*(int value) const {
        return {
            x * value,
            y * value,
            z * value,
        };
    }

    constexpr Vec operator+(Vec other) const {
        return {
            x + other.x,
            y + other.y,
            z + other.z,
        };
    }

    [[nodiscard]] constexpr Vec rotated(float angle_deg) const {

        Vec result;

        float cos_ = std::cosf(deg_to_rad(angle_deg));
        float sin_ = std::sinf(deg_to_rad(angle_deg));

        result.x = cos_ * x - sin_ * y;
        result.y = sin_ * x + cos_ * y;

        return result;
    }

    constexpr void rotate(float angle_deg) {
        Vec v = rotated(angle_deg);
        x = v.x;
        y = v.y;
    }

private:
    [[nodiscard]] static constexpr float deg_to_rad(float deg) {
        return deg * (M_PI / 180.0);
    }

};

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

        for (int x = 0; x < m_color_buffer.get_width(); ++x) {
            for (int y = 0; y < m_color_buffer.get_height(); ++y) {
                Vec p {x, y};

                int abp = edge_function(a, b, p);
                int bcp = edge_function(b, c, p);
                int cap = edge_function(c, a, p);

                bool wireframe = false;
                if (wireframe) {
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
    [[nodiscard]] static constexpr int edge_function(Vec a, Vec b, Vec c) {
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

// TODO: fix vector rotation in 3d space

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
            // t.a.rotate(1);
            // t.b.rotate(1);
            // t.c.rotate(1);
        }
    }

    for (auto&& [face_nr, face] : cube.faces | std::views::enumerate) {
        for (auto& t : face.triangles) {
            auto color = face_color_map[face_nr];
            int w = 200;
            int h = 200;
            int d = 200;
            Vec offset {width/2-w/2, height/2-h/2};
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
