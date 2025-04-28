#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Bounding_Box.h"

namespace game_logic::util::proximity
{
	bool separated
	(
		game_state::proximity::Bounding_Box const& box_0,
		game_state::proximity::Bounding_Box const& box_1
	);
}
