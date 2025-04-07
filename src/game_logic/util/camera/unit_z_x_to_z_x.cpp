#pragma once
#include "game_logic/util/camera/unit_z_x_to_z_x.h"
#include "game_logic/util/camera/unit_z_coordinate_to_z_coordinate.h"

namespace game_logic::util::camera
{
	void unit_z_x_to_z_x
	(
		GLfloat const unit_z_x, GLfloat const z,
		GLfloat* z_x
	)
	{
		unit_z_coordinate_to_z_coordinate(unit_z_x, z, z_x);
	}
}
