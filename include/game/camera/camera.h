#pragma once
#include "util/math/vector_2D.h"

namespace game::camera
{
	struct Camera
	{
		util::math::Vector_2D xy;
		GLint z;
		GLint angle;
	};
}
