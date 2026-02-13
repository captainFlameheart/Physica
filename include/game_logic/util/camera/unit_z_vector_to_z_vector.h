#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "glad_glfw.h"

namespace game_logic::util::camera
{
	void unit_z_vector_to_z_vector
	(
		GLfloat const unit_z_x, GLfloat const unit_z_y,
		GLfloat const z, 
		GLfloat* z_x, GLfloat* z_y
	);
}
#endif
