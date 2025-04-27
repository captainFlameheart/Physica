#include "game_logic/util/proximity/compute_height.h"
#include "game_logic/util/proximity/is_empty.h"

namespace game_logic::util::proximity
{
	GLuint compute_height
	(
		game_state::proximity::Tree const& tree, GLuint const max_leaf_count, 
		GLuint const node_index
	)
	{
		if (node_index < max_leaf_count)
		{
			return 1u;
		}
		game_state::proximity::Node const& node{ tree.nodes[node_index] };
		return 1u + std::max
		(
			compute_height(tree, max_leaf_count, node.children[0]), 
			compute_height(tree, max_leaf_count, node.children[1])
		);
	}

	GLuint compute_height
	(
		game_state::proximity::Tree const& tree, GLuint const max_leaf_count
	)
	{
		if (is_empty(tree))
		{
			return 0u;
		}
		return compute_height(tree, max_leaf_count, tree.root);
	}
}
