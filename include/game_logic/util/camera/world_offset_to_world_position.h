#pragma once
#include "glad_glfw.h"
#include "game_environment/environment.h"

namespace game_logic::util::camera
{
	void world_offset_to_world_position
	(
		game_environment::Environment const& environment,
		GLint const world_offset_x, GLint const world_offset_y,
		GLint* world_position_x, GLint* world_position_y
	);
}
