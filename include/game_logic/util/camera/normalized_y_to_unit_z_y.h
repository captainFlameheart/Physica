#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	void normalized_y_to_unit_z_y
	(
		game_environment::Environment const& environment,
		GLfloat normalized_y,
		GLfloat* unit_z_y
	);
}
#endif
