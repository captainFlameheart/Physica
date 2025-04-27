#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	bool has_single_node
	(
		game_state::proximity::Tree const& tree, GLuint const max_leaf_count
	);
}
