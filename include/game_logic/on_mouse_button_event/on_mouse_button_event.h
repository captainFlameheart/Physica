#pragma once
#include "game_environment/environment.h"

namespace game_logic::on_mouse_button_event
{
	void on_mouse_button_event(
		game_environment::Environment& environment,
		int const button,
		int const action,
		int const mods
	);
}
