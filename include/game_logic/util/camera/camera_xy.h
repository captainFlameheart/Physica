#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "util/math/vector_2D.h"

namespace game_logic::util::camera
{
	::util::math::Vector_2D& camera_xy(game_environment::Environment& environment);
}
