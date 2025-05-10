#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Bounding_Box.h"

namespace game_logic::util::proximity
{
	bool contains_point
	(
		game_state::proximity::Bounding_Box const& box,
		GLint const x, GLint const y
	);
}
