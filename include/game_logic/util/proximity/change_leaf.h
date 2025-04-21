#pragma once
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	inline void change_leaf
	(
		game_state::proximity::Tree& tree, GLuint const leaf_index, 
		GLuint const change_index, 
		GLint const min_x, GLint const min_y, GLint const max_x, GLint const max_y
	);
}
