#pragma once

#include <cstdint>
#include <cassert>
#include <print>
#include <vector>

#include "math.h"

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0xff;

    [[nodiscard]] static constexpr Color blue() {
        return {0x0, 0x0, 0xff, 0xff};
    }

    [[nodiscard]] static constexpr Color red() {
        return {0xff, 0x0, 0x0, 0xff};
    }

    [[nodiscard]] static constexpr Color green() {
        return {0x0, 0xff, 0x0, 0xff};
    }

    [[nodiscard]] static constexpr Color black() {
        return {0x0, 0x0, 0x0, 0xff};
    }

    [[nodiscard]] static constexpr Color white() {
        return {0xff, 0xff, 0xff, 0xff};
    }

    constexpr Color operator*(float value) const {
        return {
            static_cast<uint8_t>(r * value),
            static_cast<uint8_t>(g * value),
            static_cast<uint8_t>(b * value),
            static_cast<uint8_t>(a * value),
        };
    }

    constexpr Color operator+(Color other) const {
        return {
            static_cast<uint8_t>(r + other.r),
            static_cast<uint8_t>(g + other.g),
            static_cast<uint8_t>(b + other.b),
            static_cast<uint8_t>(a + other.a),
        };
    }

};

static_assert(sizeof(Color) == 4);

template <typename T>
class Buffer {
    const int m_width;
    const int m_height;
    std::vector<T> m_buffer;

public:
    Buffer(int width, int height)
        : m_width(width)
        , m_height(height)
        , m_buffer(m_width * m_height)
    { }

    void write(int x, int y, T value) {
        m_buffer[y * m_width + x] = value;
    }

    [[nodiscard]] T get(int x, int y) const {
        return m_buffer[y * m_width + x];
    }

    void clear(T value) {
        for (int x = 0; x < get_width(); ++x) {
            for (int y = 0; y < get_height(); ++y) {
                write(x, y, value);
            }
        }
    }

    [[nodiscard]] int get_width() const {
        return m_width;
    }

    [[nodiscard]] int get_height() const {
        return m_height;
    }

};

using ColorBuffer = Buffer<Color>;
using DepthBuffer = Buffer<float>;

class Rasterizer {
    ColorBuffer& m_color_buffer;
    DepthBuffer& m_depth_buffer;

public:
    // TODO: construct buffers inplace
    Rasterizer(ColorBuffer& color_buffer, DepthBuffer& depth_buffer)
        : m_color_buffer(color_buffer)
        , m_depth_buffer(depth_buffer)
    {
        clear();
    }

    void clear() {
        m_color_buffer.clear(Color::black());
        m_depth_buffer.clear(-std::numeric_limits<float>::max());
    }

private:
    [[nodiscard]] auto get_triangle_aabb(Vec a, Vec b, Vec c) {
        struct AABB {
            float x, y, width, height;
        } aabb;

        aabb.x = std::min({a.x, b.x, c.x});
        aabb.y = std::min({a.y, b.y, c.y});
        aabb.width = std::max({a.x, b.x, c.x});
        aabb.height = std::max({a.y, b.y, c.y});

        assert(aabb.width < m_color_buffer.get_width());
        assert(aabb.height < m_color_buffer.get_height());

        return aabb;
    }
public:

    //
    //                      (y)
    //                       1     (-z)
    //                       ^     -1
    //                       |    /
    //                       |   /
    //                       |  /
    //                       | /
    //                       |/
    // (-x) -1 -------------------------------> 1 (x)
    //                      /|
    //                     / |
    //                    /  |
    //                   /   |
    //                  /    |
    //                 1     -1
    //               (z)    (-y)
    //
    void draw_triangle(Vec a_ndc, Vec b_ndc, Vec c_ndc, Color color) {

        auto a_vp = viewport_transform(a_ndc);
        auto b_vp = viewport_transform(b_ndc);
        auto c_vp = viewport_transform(c_ndc);

        auto aabb = get_triangle_aabb(a_vp, b_vp, c_vp);

        for (float x = aabb.x; x < m_color_buffer.get_width(); ++x) {
            for (float y = aabb.y; y < m_color_buffer.get_height(); ++y) {
                Vec p { x, y, 0.0f, 1.0f };

                float abc = edge_function(a_vp, b_vp, c_vp);
                float abp = edge_function(a_vp, b_vp, p);
                float bcp = edge_function(b_vp, c_vp, p);
                float cap = edge_function(c_vp, a_vp, p);

                float weight_a = bcp / abc;
                float weight_b = cap / abc;
                float weight_c = abp / abc;

                float depth = a_vp.z * weight_a +
                              b_vp.z * weight_b +
                              c_vp.z * weight_c;

                Color col = Color::red()   * weight_a +
                            Color::green() * weight_b +
                            Color::blue()  * weight_c;

                // float stored_depth = m_depth_buffer.get(x, y);
                // if (depth < stored_depth) continue;

                // TODO: wireframe mode
                // TODO: blending


                bool show_aabb = false;

                if (abp >= 0 && bcp >= 0 && cap >= 0) {
                    m_color_buffer.write(x, y, col);
                    m_depth_buffer.write(x, y, depth);

                } else if (show_aabb) {
                    m_color_buffer.write(x, y, Color::red());
                }

            }
        }

    }

private:
    // returns the area of a triangle, which may be negative
    [[nodiscard]] static constexpr float edge_function(Vec a, Vec b, Vec c) {
        return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
    }

    // transforms coordinates from NDC to the actual viewport
    [[nodiscard]] Vec viewport_transform(Vec v) const {
        return {
            ((v.x + 1) / 2) * m_color_buffer.get_width(),
            (-(v.y - 1) / 2) * m_color_buffer.get_height(),
            v.z,
            v.w
        };

    }


};
