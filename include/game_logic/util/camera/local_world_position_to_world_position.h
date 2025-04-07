#pragma once
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	void local_world_position_to_world_position(
		game_environment::Environment const& environment,
		GLfloat const local_world_x, GLfloat const local_world_y,
		GLint* world_x, GLint* world_y
	);
}
