#pragma once
#include "game_logic/util/camera/normalized_y_to_unit_z_y.h"
#include "game_logic/util/projection/INVERSE_SCALE_Y.h"

namespace game_logic::util::camera
{
	void normalized_y_to_unit_z_y
	(
		game_environment::Environment const& environment,
		GLfloat normalized_y,
		GLfloat* unit_z_y
	)
	{
		*unit_z_y =
		(
			normalized_y * game_logic__util__projection_INVERSE_SCALE_Y(environment)
		);
	}
}

