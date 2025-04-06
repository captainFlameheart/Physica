#pragma once
#include "game_environment/Environment.h"

namespace game_logic::util::glfw
{
	void window_screen_size
	(
		game_environment::Environment const& environment,
		int* window_screen_width, int* window_screen_height
	);
}
