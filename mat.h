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
        Mat m;

        m.m[0][0] = v.x;
        m.m[1][0] = 0.0f;
        m.m[2][0] = 0.0f;
        m.m[3][0] = 0.0f;

        m.m[0][1] = 0.0f;
        m.m[1][1] = v.y;
        m.m[2][1] = 0.0f;
        m.m[3][1] = 0.0f;

        m.m[0][2] = 0.0f;
        m.m[1][2] = 0.0f;
        m.m[2][2] = v.z;
        m.m[3][2] = 0.0f;

        m.m[0][3] = 0.0f;
        m.m[1][3] = 0.0f;
        m.m[2][3] = 0.0f;
        m.m[3][3] = 1.0f;

        return m;
    }

    [[nodiscard]] static constexpr Mat translate(Vec v) {

        Mat m;

        m.m[0][0] = 1.0f;
        m.m[1][0] = 0.0f;
        m.m[2][0] = 0.0f;
        m.m[3][0] = v.x;

        m.m[0][1] = 0.0f;
        m.m[1][1] = 1.0f;
        m.m[2][1] = 0.0f;
        m.m[3][1] = v.y;

        m.m[0][2] = 0.0f;
        m.m[1][2] = 0.0f;
        m.m[2][2] = 1.0f;
        m.m[3][2] = v.z;

        m.m[0][3] = 0.0f;
        m.m[1][3] = 0.0f;
        m.m[2][3] = 0.0f;
        m.m[3][3] = 1.0f;

        return m;
    }

    [[nodiscard]] static constexpr Mat rotate(Vec axis, float angle_radians) {

        using std::sin;
        using std::cos;

        float a = angle_radians;
        Vec r = axis;
        Mat m;

        m.m[0][0] = cos(a) + r.x*r.x * (1.0f-cos(a));
        m.m[1][0] = r.x*r.y * (1.0f-cos(a)) - r.z*sin(a);
        m.m[2][0] = r.x*r.z * (1.0f-cos(a)) + r.y*sin(a);
        m.m[3][0] = 0.0f;

        m.m[0][1] = r.y*r.x * (1.0f-cos(a)) + r.z*sin(a);
        m.m[1][1] = cos(a) + r.y*r.y * (1.0f-cos(a));
        m.m[2][1] = r.y*r.z * (1.0f-cos(a)) - r.x*sin(a);
        m.m[3][1] = 0.0f;

        m.m[0][2] = r.z*r.x * (1.0f-cos(a)) - r.y*sin(a);
        m.m[1][2] = r.z*r.y * (1.0f-cos(a)) + r.x*sin(a);
        m.m[2][2] = cos(a) + r.z*r.z * (1.0f-cos(a));
        m.m[3][2] = 0.0f;

        m.m[0][3] = 0.0f;
        m.m[1][3] = 0.0f;
        m.m[2][3] = 0.0f;
        m.m[3][3] = 1.0f;

        return m;

    }

    [[nodiscard]] static constexpr Mat identity() {
        Mat m;

        m.m[0][0] = 1.0f;
        m.m[1][0] = 0.0f;
        m.m[2][0] = 0.0f;
        m.m[3][0] = 0.0f;

        m.m[0][1] = 0.0f;
        m.m[1][1] = 1.0f;
        m.m[2][1] = 0.0f;
        m.m[3][1] = 0.0f;

        m.m[0][2] = 0.0f;
        m.m[1][2] = 0.0f;
        m.m[2][2] = 1.0f;
        m.m[3][2] = 0.0f;

        m.m[0][3] = 0.0f;
        m.m[1][3] = 0.0f;
        m.m[2][3] = 0.0f;
        m.m[3][3] = 1.0f;

        return m;
    }

    // returns the nth row as a vector
    [[nodiscard]] constexpr Vec get_row(int n) const {
        return { m[0][n], m[1][n], m[2][n], m[3][n] };
    }

};
