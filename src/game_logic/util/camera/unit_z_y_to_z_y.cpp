#pragma once
#include "game_logic/util/camera/unit_z_y_to_z_y.h"
#include "game_logic/util/camera/unit_z_coordinate_to_z_coordinate.h"

namespace game_logic::util::camera
{
	void unit_z_y_to_z_y
	(
		GLfloat const unit_z_y, GLfloat const z,
		GLfloat* z_y
	)
	{
		unit_z_coordinate_to_z_coordinate(unit_z_y, z, z_y);
	}
}
