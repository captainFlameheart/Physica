#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "glad_glfw.h"
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	void unit_z_coordinate_to_camera_local_world_coordinate
	(
		game_environment::Environment const& environment,
		GLfloat const unit_z_coordinate,
		GLfloat* z_coordinate
	);
}
#endif
