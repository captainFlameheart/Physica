#pragma once
#include "game_environment/environment.h"

namespace game_logic::on_scroll_event
{
	void on_scroll_event(
		game_environment::Environment& environment,
		double x_offset,
		double y_offset
	);
}
