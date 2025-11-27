#pragma once

#include <cstdint>

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

    constexpr Color& operator+=(Color other) {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        return *this;
    }

    constexpr Color operator/(int value) const {
        return {
            static_cast<uint8_t>(r / value),
            static_cast<uint8_t>(g / value),
            static_cast<uint8_t>(b / value),
            static_cast<uint8_t>(a / value),
        };
    }

};
