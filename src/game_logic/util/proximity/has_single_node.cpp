#include "game_logic/util/proximity/has_single_node.h"

namespace game_logic::util::proximity
{
	bool has_single_node
	(
		game_state::proximity::Tree const& tree, GLuint const max_leaf_count
	)
	{
		return tree.root < max_leaf_count;
	}
}
