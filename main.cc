#include <print>
#include <vector>

namespace rl {
#include <raylib.h>
}

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0xff;
};

static_assert(sizeof(Color) == 4);

class ColorBuffer {
    const size_t m_width = 500;
    const size_t m_height = 500;
    std::vector<Color> m_color_buffer{ m_width * m_height };

public:
    ColorBuffer() = default;

    void write(int x, int y, Color color) {
        m_color_buffer[y * m_width + x] = color;
    }

    [[nodiscard]] Color get(int x, int y) const {
        return m_color_buffer[y * m_width + x];
    }

    [[nodiscard]] size_t get_width() const {
        return m_width;
    }

    [[nodiscard]] size_t get_height() const {
        return m_height;
    }

};

int main() {

    ColorBuffer color_buffer;

    for (size_t x = 0; x < color_buffer.get_width(); ++x) {
        for (size_t y = 0; y < color_buffer.get_height(); ++y) {
            color_buffer.write(x, y, Color(0xff, 0x0, 0x0, 0xff));
        }
    }

    rl::InitWindow(color_buffer.get_width(), color_buffer.get_width(), "ras");

    while (!rl::WindowShouldClose()) {
        rl::BeginDrawing();
        rl::ClearBackground(rl::BLACK);

        for (size_t x = 0; x < color_buffer.get_width(); ++x) {
            for (size_t y = 0; y < color_buffer.get_height(); ++y) {
                rl::Color c = std::bit_cast<rl::Color>(color_buffer.get(x, y));
                rl::DrawRectangle(x, y, 1, 1, c);
            }
        }

        rl::EndDrawing();
    }

    rl::CloseWindow();

}
