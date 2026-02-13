#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "glad_glfw.h"
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	GLfloat camera_z(game_environment::Environment const& environment);
}
#endif
