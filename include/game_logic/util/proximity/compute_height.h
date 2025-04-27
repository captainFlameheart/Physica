#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	GLuint compute_height
	(
		game_state::proximity::Tree const& tree, GLuint const max_leaf_count
	);
}
