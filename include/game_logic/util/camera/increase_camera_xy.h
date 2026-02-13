#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "glad_glfw.h"
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	void increase_camera_xy
	(
		game_environment::Environment& environment,
		GLint const x_increase, GLint const y_increase
	);
}
#endif
