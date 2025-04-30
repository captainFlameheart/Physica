#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"

namespace game_logic::util::proximity
{
	void print_leaf_contacts
	(
		std::ostream& os,
		game_state::proximity::Tree const& tree, 
		GLuint const leaf_index
	);
}
