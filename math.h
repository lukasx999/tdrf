#pragma once

#include <array>
#include <cmath>

#include "mat.h"
#include "vec.h"

[[nodiscard]] inline constexpr float deg_to_rad(float deg) {
    return deg * (M_PI / 180.0);
}

[[nodiscard]] inline constexpr float rad_to_deg(float rad) {
    return rad * (180.0 / M_PI);
}

// TODO: tests for math module
