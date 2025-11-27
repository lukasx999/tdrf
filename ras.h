#pragma once

#include <ranges>
#include <memory>
#include <print>
#include <unordered_map>
#include <random>
#include <vector>
#include <cstdint>
#include <cassert>

#include "math.h"
#include "Color.h"
#include "Buffer.h"
#include "Framebuffer.h"

struct Rectangle {
    float x, y, width, height;
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

enum class WindingOrder { Clockwise, CounterClockwise };
enum class CullMode { Front, Back, None };

class Rasterizer {
    Framebuffer& m_framebuffer;
    // vertex winding order of front face triangles
    WindingOrder m_winding_order = WindingOrder::CounterClockwise;
    CullMode m_cull_mode = CullMode::None;

public:
    explicit Rasterizer(Framebuffer& framebuffer) : m_framebuffer(framebuffer) {
        m_framebuffer.clear();
    }

    [[nodiscard]] Framebuffer& get_framebuffer() const {
        return m_framebuffer;
    }

    [[nodiscard]] CullMode get_cull_mode() const {
        return m_cull_mode;
    }

    void set_cull_mode(CullMode cull_mode) {
        m_cull_mode = cull_mode;
    }

    [[nodiscard]] WindingOrder get_winding_order() const {
        return m_winding_order;
    }

    void set_winding_order(WindingOrder winding_order) {
        m_winding_order = winding_order;
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

        // TODO: double buffering
        // TODO: thread pool
        // TODO: MSAA
        // TODO: improve code structure (framebuffer)
        // TODO: run rasterizer in parallel
        // TODO: reconstruct triangles that have a vertex off-screen
        // TODO: vertex shader outputs

        for (float x = aabb.x; x < aabb.width; ++x) {
            for (float y = aabb.y; y < aabb.height; ++y) {
                Vec p { x, y, 0.0f, 1.0f };
                rasterize_pixel(p, a_vp, b_vp, c_vp, fs);
            }
        }

    }

private:
    void rasterize_pixel(Vec p, Vec a_vp, Vec b_vp, Vec c_vp, FragmentShader fs) {

        // TODO: fix msaa
        // int samples = 4;
        // assert(is_power_of_2(samples));
        // assert(samples != 2);

        // std::vector<Color> colors;
        // for (int i = 1; i <= samples; ++i) {

            // switch (i) {
            //     case 1:
            //         p.x = p.x - 0.5;
            //         p.y = p.y - 0.5;
            //         break;
            //     case 2:
            //         p.x = p.x + 0.5;
            //         p.y = p.y - 0.5;
            //         break;
            //     case 3:
            //         p.x = p.x - 0.5;
            //         p.y = p.y + 0.5;
            //         break;
            //     case 4:
            //         p.x = p.x + 0.5;
            //         p.y = p.y + 0.5;
            //         break;
            // }

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

            float stored_depth = m_framebuffer.get_depth_buffer().get(p.x, p.y);
            if (depth < stored_depth) return;

            Color color_debug = interpolate_value(Color::red(), Color::green(), Color::blue());

            bool show_aabb = false;

            // test if the current pixel is inside of the triangle
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

            auto [front, back] = get_faces_from_winding_order(cw, ccw);

            bool should_render = apply_culling(front, back);

            if (should_render) {
                Color color = fs(p);
                Color stored_color = m_framebuffer.get_color_buffer().get(p.x, p.y);
                Color result = blend_colors(color, stored_color);
                m_framebuffer.get_color_buffer().write(p.x, p.y, color_debug);
                m_framebuffer.get_depth_buffer().write(p.x, p.y, depth);
                // colors.push_back(result);

            } else if (show_aabb) {
                m_framebuffer.get_color_buffer().write(p.x, p.y, Color::red());

            }
            // else {
            //     colors.push_back(Color(0x0, 0x0, 0x0, 0x0));
            // }

        // // cant use color struct for summing up color values, due to integer overflow
        // int r = 0;
        // int g = 0;
        // int b = 0;
        // int a = 0;
        //
        // for (auto& color : colors) {
        //     r += color.r;
        //     g += color.g;
        //     b += color.b;
        //     a += color.a;
        // }
        //
        // Color color(r/samples, g/samples, b/samples, a/samples);
        // m_color_buffer.write(p.x, p.y, color);

    }

    // returns the area of a triangle, which may be negative
    [[nodiscard]] static constexpr float triangle_signed_area(Vec a, Vec b, Vec c) {
        return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
    }

    // transforms coordinates from NDC to the actual viewport
    [[nodiscard]] Vec viewport_transform(Vec v) const {
        return {
            ((v.x + 1) / 2) * m_framebuffer.get_width(),
            (-(v.y - 1) / 2) * m_framebuffer.get_height(),
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

        assert(aabb.width <= m_framebuffer.get_width());
        assert(aabb.height <= m_framebuffer.get_height());

        return aabb;
    }

    [[nodiscard]] bool apply_culling(bool front, bool back) const {
        switch (m_cull_mode) {
            using enum CullMode;
            case Front: return back;
            case Back: return front;
            case None: return front || back;
            default: assert(!"invalid cull mode");
        }
    }

    [[nodiscard]] std::tuple<bool, bool>
    get_faces_from_winding_order(bool cw, bool ccw) const {

        bool front, back;
        switch (m_winding_order) {
            using enum WindingOrder;

            case Clockwise:
                front = cw;
                back = ccw;
                break;

            case CounterClockwise:
                front = ccw;
                back = cw;
                break;

            default: assert(!"invalid winding order");
        }

        return {front, back};
    }

    [[nodiscard]] static constexpr Color blend_colors(Color src, Color dest) {
        // TODO: allow setting custom blend functions
        float factor_src = src.a / 255.0f;
        float factor_dest = 1.0f - factor_src;
        return src * factor_src + dest * factor_dest;
    }

    [[nodiscard]] static constexpr bool is_power_of_2(int value) {
        return value && !(value & (value-1));
    }

};
