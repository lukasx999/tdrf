#pragma once

#include <stdexcept>
#include <format>
#include <array>
#include <cmath>

struct Vec {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    constexpr float& operator[](int i) {
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
        throw std::runtime_error("invalid vector access");
    }

    constexpr float operator[](int i) const {
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
        throw std::runtime_error("invalid vector access");
    }

    constexpr bool operator<=>(const Vec&) const = default;

    constexpr Vec operator/(float value) const {
        return {
            x / value,
            y / value,
            z / value,
            w / value,
        };
    }

    constexpr float operator*(Vec other) const {
        return
            x * other.x +
            y * other.y +
            z * other.z +
            w * other.w;
    }

    constexpr Vec operator*(float value) const {
        return {
            x * value,
            y * value,
            z * value,
            w * value,
        };
    }

    constexpr Vec& operator*=(float value) {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        return *this;
    }

    constexpr Vec operator+(Vec other) const {
        return {
            x + other.x,
            y + other.y,
            z + other.z,
            w + other.w,
        };
    }

    constexpr Vec operator+(float value) const {
        return {
            x + value,
            y + value,
            z + value,
            w + value,
        };
    }

    constexpr Vec operator-(Vec other) const {
        return {
            x - other.x,
            y - other.y,
            z - other.z,
            w - other.w,
        };
    }

    constexpr Vec operator-(float value) const {
        return {
            x - value,
            y - value,
            z - value,
            w - value,
        };
    }

    // constexpr Vec& operator*=(Mat mat) {
    //     // TODO:
    //     return *this;
    // }

    [[nodiscard]] constexpr Vec cross(Vec other) const {
        return {
            y*other.z - z*other.y,
            z*other.x - x*other.z,
            x*other.y - y*other.x,
            w,
        };
    }

    [[nodiscard]] constexpr float dot(Vec other) const {
        return *this * other;
    }

    [[nodiscard]] constexpr float length() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    [[nodiscard]] constexpr Vec normalized() const {
        return {
            x / length(),
            y / length(),
            z / length(),
            w,
        };
    }

};

template <>
struct std::formatter<Vec> : std::formatter<std::string> {
    auto format(Vec v, std::format_context& ctx) const {
        auto fmt = std::format("{{ x={}, y={}, z={}, w={} }}", v.x, v.y, v.z, v.w);;
        return std::formatter<std::string>::format(fmt, ctx);
    }
};
