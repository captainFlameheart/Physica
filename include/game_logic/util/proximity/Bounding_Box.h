#pragma once
#include "util/math/vector_2D.h"

namespace game_logic::util::proximity
{
	struct Bounding_Box
	{
		util::math::Vector_2D min;
		util::math::Vector_2D max;
	};
}
