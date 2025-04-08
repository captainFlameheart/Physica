#pragma once
#include "glad_glfw.h"
#include "util/math/vector_2D.h"

namespace game_logic::util::rigid_body
{
	struct Position
	{
		::util::math::Vector_2D position;
		GLint angle;
	};
}
