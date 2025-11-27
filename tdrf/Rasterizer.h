#pragma once

#include <cassert>
#include <ranges>

#include "Vec.h"
#include "Color.h"
#include "Framebuffer.h"
#include "types.h"

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
    void draw_triangle(Vec a_ndc, Vec b_ndc, Vec c_ndc, VertexShader vs, FragmentShader fs);

private:
    void rasterize_pixel(Vec p, Vec a_vp, Vec b_vp, Vec c_vp, FragmentShader fs);

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
