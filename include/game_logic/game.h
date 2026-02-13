#pragma once
#include "game_environment/environment.h"
#include "macros.h"

namespace game_logic
{
	void _initialize(game_environment::Environment &environment);

	void _on_framebuffer_size_changed(
		game_environment::Environment& environment,
		int width, int height
	);

	void _on_key_event(
		game_environment::Environment& environment,
		int const key,
		int const scancode,
		int const action,
		int const mods
	);

	void _on_cursor_event(
		game_environment::Environment& environment,
		double const x_pos,
		double const y_pos
	);

	void _on_mouse_button_event(
		game_environment::Environment& environment,
		int const button,
		int const action,
		int const mods
	);

	void _on_scroll_event(
		game_environment::Environment& environment, 
		double const x_offset,
		double const y_offset
	);

	void _tick(game_environment::Environment& environment);

	void _draw(game_environment::Environment& environment);

	void _free(game_environment::Environment& environment);
}
