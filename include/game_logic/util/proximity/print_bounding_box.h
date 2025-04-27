#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Bounding_Box.h"

namespace game_logic::util::proximity
{
	std::ostream& print(std::ostream& os, game_state::proximity::Bounding_Box const& box);
}
