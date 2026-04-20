#pragma once
#include "glad_glfw.h"
#include <bit>

constexpr GLuint minus_1_uint{ static_cast<GLuint>(-1) };
constexpr GLuint null_uint{ minus_1_uint };

/*
* Assumption: n >= 2
*/
constexpr GLuint ceil_log2_n_minus_1(GLuint n) {
    return std::bit_width(n - 2);
}
