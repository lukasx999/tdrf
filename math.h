#pragma once

#include <stdexcept>
#include <array>
#include <cmath>

[[nodiscard]] inline constexpr float deg_to_rad(float deg) {
    return deg * (M_PI / 180.0);
}

[[nodiscard]] inline constexpr float rad_to_deg(float rad) {
    return rad * (180.0 / M_PI);
}

struct Vec {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

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

};

struct Mat {
    using Column = Vec;
    std::array<Column, 4> m;

    // explicit constexpr Mat(float v) : m {
    //     Vec { v, 0, 0, 0 },
    //     Vec { 0, v, 0, 0 },
    //     Vec { 0, 0, v, 0 },
    //     Vec { 0, 0, 0, v },
    // }
    // { }

    constexpr Mat operator*(float v) const {
        return {
            m[0] * v,
            m[1] * v,
            m[2] * v,
            m[3] * v,
        };
    }

    constexpr Mat operator/(float v) const {
        return {
            m[0] / v,
            m[1] / v,
            m[2] / v,
            m[3] / v,
        };
    }

    constexpr Mat operator+(float v) const {
        return {
            m[0] + v,
            m[1] + v,
            m[2] + v,
            m[3] + v,
        };
    }

    constexpr Mat operator-(float v) const {
        return {
            m[0] - v,
            m[1] - v,
            m[2] - v,
            m[3] - v,
        };
    }

    constexpr Mat operator+(Mat other) const {
        return {
            m[0] + other.m[0],
            m[1] + other.m[1],
            m[2] + other.m[2],
            m[3] + other.m[3],
        };
    }

    constexpr Mat operator-(Mat other) const {
        return {
            m[0] - other.m[0],
            m[1] - other.m[1],
            m[2] - other.m[2],
            m[3] - other.m[3],
        };
    }

    constexpr Mat operator*(Mat other) const {
        return {
            // Vec { m[0][0] },
        };
    }

    [[nodiscard]] constexpr Vec operator*(Vec v) const {
        Vec row0 { m[0][0], m[1][0], m[2][0], m[3][0] };
        Vec row1 { m[0][1], m[1][1], m[2][1], m[3][1] };
        Vec row2 { m[0][2], m[1][2], m[2][2], m[3][2] };
        Vec row3 { m[0][3], m[1][3], m[2][3], m[3][3] };

        return {
            row0 * v,
            row1 * v,
            row2 * v,
            row3 * v,
        };
    }

    [[nodiscard]] static constexpr Mat scale(Vec v) {
        return {
            Vec { v.x, 0, 0, 0 },
            Vec { 0, v.y, 0, 0 },
            Vec { 0, 0, v.z, 0 },
            Vec { 0, 0, 0, 1 },
        };
    }

    [[nodiscard]] static constexpr Mat translate(Vec v) {
        return {
            Vec { 0, 0, 0, v.x },
            Vec { 0, 0, 0, v.y },
            Vec { 0, 0, 0, v.z },
            Vec { 0, 0, 0, 1 },
        };
    }

    [[nodiscard]] static constexpr Mat rotate(Vec r, float a) {
        return {
            Vec { std::cos(a)+std::pow(r.x, 2.0f)*(1.0f-std::cos(a)), r.x*r.y*(1.0f-std::cos(a))-r.z*std::sin(a), r.x*r.z*(1.0f-std::cos(a))+r.y*std::sin(a), 0.0f },
            Vec { r.y*r.x*(1.0f-std::cos(a))+r.z*std::sin(a), std::cos(a)+std::pow(r.y, 2.0f)+(1.0f-std::cos(a)), r.y*r.z*(1.0f-std::cos(a))-r.x*std::sin(a), 0.0f },
            Vec { r.z*r.x*(1.0f-std::cos(a))-r.y*std::sin(a), r.z*r.y*(1.0f-std::cos(a))+r.x*std::sin(a), std::cos(a)+std::pow(r.z, 2.0f)*(1.0f-std::cos(a)) },
            Vec { 0.0f, 0.0f, 0.0f, 1.0f },
        };
    }

};
