#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "glad_glfw.h"

namespace game_logic::util::camera
{
	void unit_z_coordinate_to_z_coordinate
	(
		GLfloat const unit_z_coordinate, GLfloat const z,
		GLfloat* z_coordinate
	);
}
#endif
