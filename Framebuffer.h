#pragma once

#include "Buffer.h"

class Framebuffer {
    const int m_width;
    const int m_height;
    ColorBuffer m_color_buffer {m_width, m_height};
    DepthBuffer m_depth_buffer {m_width, m_height};

public:
    Framebuffer(int width, int height)
        : m_width(width)
        , m_height(height)
    { }

    [[nodiscard]] int get_width() const {
        return m_width;
    }

    [[nodiscard]] int get_height() const {
        return m_height;
    }

    [[nodiscard]] ColorBuffer& get_color_buffer() {
        return m_color_buffer;
    }

    [[nodiscard]] const ColorBuffer& get_color_buffer() const {
        return m_color_buffer;
    }

    [[nodiscard]] DepthBuffer& get_depth_buffer() {
        return m_depth_buffer;
    }

    [[nodiscard]] const DepthBuffer& get_depth_buffer() const {
        return m_depth_buffer;
    }

    void clear() {
        m_color_buffer.clear(Color::black());
        m_depth_buffer.clear(-1.0f);
    }

};
