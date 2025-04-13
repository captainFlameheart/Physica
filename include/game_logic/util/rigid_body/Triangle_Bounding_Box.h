#pragma once
#include "glad_glfw.h"
#include "util/math/vector_2D.h"

namespace game_logic::util::rigid_body
{
	struct Triangle_Bounding_Box
	{
		::util::math::Vector_2D min;
		::util::math::Vector_2D max;
	};
}
