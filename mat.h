#pragma once

#include <array>
#include <cmath>

#include "vec.h"

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

        Vec row0 = get_row(0);
        Vec row1 = get_row(1);
        Vec row2 = get_row(2);
        Vec row3 = get_row(3);

        return {
            Vec {row0 * other.m[0], row0 * other.m[1], row0 * other.m[2], row0 * other.m[3] },
            Vec {row1 * other.m[0], row1 * other.m[1], row1 * other.m[2], row1 * other.m[3] },
            Vec {row2 * other.m[0], row2 * other.m[1], row2 * other.m[2], row2 * other.m[3] },
            Vec {row3 * other.m[0], row3 * other.m[1], row3 * other.m[2], row3 * other.m[3] },
        };
    }

    [[nodiscard]] constexpr Vec operator*(Vec v) const {

        Vec row0 = get_row(0);
        Vec row1 = get_row(1);
        Vec row2 = get_row(2);
        Vec row3 = get_row(3);

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

    // returns the nth row as a vector
    [[nodiscard]] constexpr Vec get_row(int n) const {
        return { m[0][n], m[1][n], m[2][n], m[3][n] };
    }

};
