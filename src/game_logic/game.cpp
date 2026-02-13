#include "glad_glfw.h"
#include "game_logic/game.h"
#include "legacy/game_logic/game.h"

#define RUN_LEGACY 1

namespace game_logic
{
	void initialize(game_environment::Environment& environment)
	{
#if RUN_LEGACY
		::legacy::game_logic::initialize(environment);
#else
#endif
	}

	void on_framebuffer_size_changed(
		game_environment::Environment& environment,
		int width, int height
	)
	{
#if RUN_LEGACY
		::legacy::game_logic::on_framebuffer_size_changed
		(
			environment,
			width, height
		);
#else
#endif
	}

	void on_key_event(
		game_environment::Environment& environment,
		int const key,
		int const scancode,
		int const action,
		int const mods
	)
	{
#if RUN_LEGACY
		::legacy::game_logic::on_key_event
		(
			environment,
			key,
			scancode,
			action,
			mods
		);
#else
#endif
	}

	void on_cursor_event(
		game_environment::Environment& environment,
		double const x_pos,
		double const y_pos
	)
	{
#if RUN_LEGACY
		::legacy::game_logic::on_cursor_event
		(
			environment,
			x_pos,
			y_pos
		);
#else
#endif
	}

	void on_mouse_button_event(
		game_environment::Environment& environment,
		int const button,
		int const action,
		int const mods
	)
	{
#if RUN_LEGACY
		::legacy::game_logic::on_mouse_button_event
		(
			environment,
			button,
			action,
			mods
		);
#else
#endif
	}

	void on_scroll_event(
		game_environment::Environment& environment,
		double const x_offset,
		double const y_offset
	)
	{
#if RUN_LEGACY
		::legacy::game_logic::on_scroll_event
		(
			environment,
			x_offset,
			y_offset
		);
#else
#endif
	}

	void tick(game_environment::Environment& environment)
	{
#if RUN_LEGACY
		::legacy::game_logic::tick(environment);
#else
#endif
	}

	void render(game_environment::Environment& environment)
	{
#if RUN_LEGACY
		::legacy::game_logic::render(environment);
#else
#endif
	}

	void free(game_environment::Environment& environment)
	{
#if RUN_LEGACY
		::legacy::game_logic::free(environment);
#else
#endif
	}
}
