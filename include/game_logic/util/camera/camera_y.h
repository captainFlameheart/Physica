#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "glad_glfw.h"
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	GLint& camera_y(game_environment::Environment& environment);
}
#endif
