#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Bounding_Box.h"

namespace game_logic::util::proximity
{
	GLint compute_cost(game_state::proximity::Bounding_Box const& bounding_box);
}
