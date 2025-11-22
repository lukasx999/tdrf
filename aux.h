#pragma once

#include "math.h"

struct Triangle {
    Vec a;
    Vec b;
    Vec c;
};

struct Face {
    std::array<Triangle, 2> triangles;
};

struct Cube {
    std::array<Face, 6> faces;
};

inline constexpr Cube cube {

    Face {
        Triangle {
            Vec(-0.5f, -0.5f, -0.5f, 1.0f),
            Vec( 0.5f, -0.5f, -0.5f, 1.0f),
            Vec( 0.5f,  0.5f, -0.5f, 1.0f),
        },
        Triangle {
            Vec( 0.5f,  0.5f, -0.5f, 1.0f),
            Vec(-0.5f,  0.5f, -0.5f, 1.0f),
            Vec(-0.5f, -0.5f, -0.5f, 1.0f),
        },
    },

    Face {
        Triangle {
            Vec(-0.5f, -0.5f,  0.5f, 1.0f),
            Vec( 0.5f, -0.5f,  0.5f, 1.0f),
            Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        },
        Triangle {
            Vec( 0.5f,  0.5f,  0.5f, 1.0f),
            Vec(-0.5f,  0.5f,  0.5f, 1.0f),
            Vec(-0.5f, -0.5f,  0.5f, 1.0f),
        },
    },

    Face {
        Triangle {
            Vec(-0.5f,  0.5f,  0.5f, 1.0f),
            Vec(-0.5f,  0.5f, -0.5f, 1.0f),
            Vec(-0.5f, -0.5f, -0.5f, 1.0f),
        },
        Triangle {
            Vec(-0.5f, -0.5f, -0.5f, 1.0f),
            Vec(-0.5f, -0.5f,  0.5f, 1.0f),
            Vec(-0.5f,  0.5f,  0.5f, 1.0f),
        },
    },

    Face {
        Triangle {
            Vec( 0.5f,  0.5f,  0.5f, 1.0f),
            Vec( 0.5f,  0.5f, -0.5f, 1.0f),
            Vec( 0.5f, -0.5f, -0.5f, 1.0f),
        },
        Triangle {
            Vec( 0.5f, -0.5f, -0.5f, 1.0f),
            Vec( 0.5f, -0.5f,  0.5f, 1.0f),
            Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        }
    },

    Face {
        Triangle {
            Vec(-0.5f, -0.5f, -0.5f, 1.0f),
            Vec( 0.5f, -0.5f, -0.5f, 1.0f),
            Vec( 0.5f, -0.5f,  0.5f, 1.0f),
        },
        Triangle {
            Vec( 0.5f, -0.5f,  0.5f, 1.0f),
            Vec(-0.5f, -0.5f,  0.5f, 1.0f),
            Vec(-0.5f, -0.5f, -0.5f, 1.0f),
        }
    },

    Face {
        Triangle {
            Vec(-0.5f,  0.5f, -0.5f, 1.0f),
            Vec( 0.5f,  0.5f, -0.5f, 1.0f),
            Vec( 0.5f,  0.5f,  0.5f, 1.0f),
        },
        Triangle {
            Vec( 0.5f,  0.5f,  0.5f, 1.0f),
            Vec(-0.5f,  0.5f,  0.5f, 1.0f),
            Vec(-0.5f,  0.5f, -0.5f, 1.0f),
        }
    }
};
