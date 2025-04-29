#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	void initialize
	(
		game_state::proximity::Tree& tree, GLuint const max_leaf_count, 
		GLuint const max_contact_count
	);
}
