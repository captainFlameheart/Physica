#pragma once
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	void increase_camera_angle
	(
		game_environment::Environment& environment,
		GLint const angle_increase
	);
}
