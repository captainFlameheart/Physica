#pragma once
#include <GLFW/glfw3.h>
#include "game_environment/environment.h"
#include "macros.h"

namespace game
{
	void initialize(game_environment::Environment &environment);

	void on_key_event(
		game_environment::Environment& environment,
		int const key,
		int const scancode,
		int const action,
		int const mods
	);

	void on_cursor_event(
		game_environment::Environment& environment,
		double const x_pos,
		double const y_pos
	);

	void on_mouse_button_event(
		game_environment::Environment& environment,
		int const button,
		int const action,
		int const mods
	);

	void on_scroll_event(
		game_environment::Environment& environment, 
		double const x_offset,
		double const y_offset
	);

	void tick(game_environment::Environment& environment);

	void render(game_environment::Environment& environment);

	void free(game_environment::Environment& environment);
}
