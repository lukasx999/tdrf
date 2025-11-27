#pragma once

#include "Color.h"
#include "Vec.h"

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
