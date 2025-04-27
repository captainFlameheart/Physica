#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	void insert_leaf_to_empty_tree
	(
		game_state::proximity::Tree& tree, GLuint const max_leaf_count,
		GLuint const leaf_index,
		GLint const min_x, GLint const min_y, GLint const max_x, GLint const max_y
	);
}
