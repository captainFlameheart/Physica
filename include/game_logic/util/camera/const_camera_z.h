#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	GLfloat camera_z(game_environment::Environment const& environment);
}
