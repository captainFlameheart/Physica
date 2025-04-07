#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	GLint& camera_x(game_environment::Environment& environment);
}
