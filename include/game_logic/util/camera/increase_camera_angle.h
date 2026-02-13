#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	void increase_camera_angle
	(
		game_environment::Environment& environment,
		GLint const angle_increase
	);
}
#endif
