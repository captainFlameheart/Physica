#pragma once
#include "game_environment/environment.h"

namespace game_logic::util::glfw
{
	void window_screen_cursor_position
	(
		game_environment::Environment const& environment,
		double* window_screen_cursor_x, double* window_screen_cursor_y
	);
}
