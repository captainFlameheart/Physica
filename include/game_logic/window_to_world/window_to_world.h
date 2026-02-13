#pragma once
#include "RUN_LEGACY.h"
#if RUN_LEGACY == 1
#include "game_environment/environment.h"

namespace game_logic::window_to_world
{
	void window_screen_position_to_world_vector
	(
		game_environment::Environment const& environment,
		GLfloat const window_screen_x, GLfloat const window_screen_y,
		GLint* world_x, GLint* world_y
	);

	void window_screen_cursor_position_to_world_vector
	(
		game_environment::Environment const& environment,
		GLint* world_x, GLint* world_y
	);

	void window_screen_position_to_world_position
	(
		game_environment::Environment const& environment,
		double const window_screen_x, double const window_screen_y,
		GLint* world_x, GLint* world_y
	);

	void window_screen_cursor_position_to_world_position
	(
		game_environment::Environment const& environment,
		GLint* world_x, GLint* world_y
	);
}
#endif
