#pragma once
#include "glad_glfw.h"

namespace game_logic::util::camera
{
	void unit_z_x_to_z_x
	(
		GLfloat const unit_z_x, GLfloat const z,
		GLfloat* z_x
	);
}
