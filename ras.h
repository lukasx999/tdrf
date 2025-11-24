#pragma once

#include <ranges>
#include <print>
#include <random>
#include <vector>
#include <cstdint>
#include <cassert>

#include "math.h"

struct Rectangle {
    float x, y, width, height;
};

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

using VertexShader = Vec(Vec);
using FragmentShader = Color(Vec);

[[nodiscard]] inline Vec default_vertex_shader(Vec pos) {
    return pos;
}

[[nodiscard]] inline Color default_fragment_shader(Vec) {
    return Color::blue();
}

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
        m_depth_buffer.clear(-1.0f);
    }

public:
    void render_vertex_buffer(std::span<const Vec> vertices, VertexShader vs, FragmentShader fs) {

        assert(vertices.size() % 3 == 0);
        for (auto&& [idx, verts] : vertices | std::views::chunk(3) | std::views::enumerate) {

            assert(verts.size() == 3);
            const Vec& a = verts[0];
            const Vec& b = verts[1];
            const Vec& c = verts[2];

            draw_triangle(a, b, c, vs, fs);
        }
    }

    //
    //                (y)
    //                 1 (-z)
    //                 ^  -1
    //                 |  /
    //                 | /
    //                 |/
    // (-x) -1 -----------------> 1 (x)
    //                /|
    //               / |
    //              /  |
    //             1  -1
    //            (z)(-y)
    //
    void draw_triangle(Vec a_ndc, Vec b_ndc, Vec c_ndc, VertexShader vs, FragmentShader fs) {

        // TODO: clip vertices outside of ndc area, and reconstruct triangle
        // TODO: divide by w

        a_ndc = vs(a_ndc);
        b_ndc = vs(b_ndc);
        c_ndc = vs(c_ndc);

        // TODO: fix z values, they should go from 0.0 to 1.0
        Vec a_vp = viewport_transform(a_ndc);
        Vec b_vp = viewport_transform(b_ndc);
        Vec c_vp = viewport_transform(c_ndc);

        auto aabb = get_triangle_aabb(a_vp, b_vp, c_vp);

        for (float x = aabb.x; x < aabb.width; ++x) {
            for (float y = aabb.y; y < aabb.height; ++y) {
                Vec p { x, y, 0.0f, 1.0f };

                float abc = triangle_signed_area(a_vp, b_vp, c_vp);
                float abp = triangle_signed_area(a_vp, b_vp, p);
                float bcp = triangle_signed_area(b_vp, c_vp, p);
                float cap = triangle_signed_area(c_vp, a_vp, p);

                float weight_a = bcp / abc;
                float weight_b = cap / abc;
                float weight_c = abp / abc;

                auto interpolate_value = [&]<typename T>(T a, T b, T c) {
                    return a * weight_a + b * weight_b + c * weight_c;
                };

                float depth = interpolate_value(a_vp.z, b_vp.z, c_vp.z);

                float stored_depth = m_depth_buffer.get(x, y);
                if (depth < stored_depth) continue;

                Color color_debug = interpolate_value(Color::red(), Color::green(), Color::blue());

                // TODO: MSAA
                // TODO: run rasterizer in parallel
                // TODO: cull modes
                // TODO: vertex shader outputs
                // TODO: blending

                bool show_aabb = false;

                bool ccw = abp <= 0 &&
                           bcp <= 0 &&
                           cap <= 0;

                bool cw = abp >= 0 &&
                          bcp >= 0 &&
                          cap >= 0;

                // TODO: wireframe mode

                // bool ccw = (weight_a <= 0.01 && weight_a >= 0) ||
                //            (weight_b <= 0.01 && weight_b >= 0) ||
                //            (weight_c <= 0.01 && weight_c >= 0);

                // bool ccw = (abp <= 0 && abp >= -500) ||
                //            (bcp <= 0 && bcp >= -500) ||
                //            (cap <= 0 && cap >= -500);

                if (ccw || cw) {
                    // Color color = fs(p);
                    m_color_buffer.write(x, y, color_debug);
                    m_depth_buffer.write(x, y, depth);

                } else if (show_aabb) {
                    m_color_buffer.write(x, y, Color::red());
                }

            }
        }

    }

private:
    // returns the area of a triangle, which may be negative
    [[nodiscard]] static constexpr float triangle_signed_area(Vec a, Vec b, Vec c) {
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

    [[nodiscard]] Rectangle get_triangle_aabb(Vec a, Vec b, Vec c) {

        Rectangle aabb;

        aabb.x = std::min({a.x, b.x, c.x});
        aabb.y = std::min({a.y, b.y, c.y});
        aabb.width = std::max({a.x, b.x, c.x});
        aabb.height = std::max({a.y, b.y, c.y});

        assert(aabb.width < m_color_buffer.get_width());
        assert(aabb.height < m_color_buffer.get_height());

        return aabb;
    }

};
