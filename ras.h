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

    void draw_triangle(Vec a, Vec b, Vec c, Color color) {

        float aabb_x = std::min({a.x, b.x, c.x});
        float aabb_y = std::min({a.y, b.y, c.y});
        float aabb_width = std::max({a.x, b.x, c.x});
        float aabb_height = std::max({a.y, b.y, c.y});

        assert(aabb_width < m_color_buffer.get_width());
        assert(aabb_height < m_color_buffer.get_height());

        for (float x = aabb_x; x < aabb_width; ++x) {
            for (float y = aabb_y; y < aabb_height; ++y) {
                Vec p { x, y, 0.0f, 1.0f };

                int abc = edge_function(a, b, c);
                int abp = edge_function(a, b, p);
                int bcp = edge_function(b, c, p);
                int cap = edge_function(c, a, p);

                float weight_a = static_cast<float>(bcp) / abc;
                float weight_b = static_cast<float>(cap) / abc;
                float weight_c = static_cast<float>(abp) / abc;

                float depth = a.z * weight_a +
                              b.z * weight_b +
                              c.z * weight_c;

                Color col = Color::red()   * weight_a +
                            Color::green() * weight_b +
                            Color::blue()  * weight_c;

                float stored_depth = m_depth_buffer.get(x, y);

                // TODO: wireframe mode
                // TODO: blending

                if (depth < stored_depth) continue;

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

};
