#include "game_logic/util/proximity/print_tree.h"
#include "game_logic/util/proximity/print_bounding_box.h"
#include "game_logic/util/proximity/is_empty.h"

namespace game_logic::util::proximity
{
	std::ostream& print
	(
		std::ostream& os, 
		game_state::proximity::Tree const& tree, GLuint const max_leaf_count, 
		GLuint depth, GLuint node_index
	)
	{
		for (GLuint i{ 0 }; i < depth; ++i)
		{
			os << '\t';
		}
		game_state::proximity::Node const& node{ tree.nodes[node_index] };
		print(os, node.bounding_box) << '\n';
		if (node_index >= max_leaf_count)
		{
			print(os, tree, max_leaf_count, depth + 1u, node.children[0]);
			print(os, tree, max_leaf_count, depth + 1u, node.children[1]);
		}
		return os;
	}

	std::ostream& print_tree
	(
		std::ostream& os, 
		game_state::proximity::Tree const& tree, GLuint const max_leaf_count
	)
	{
		if (is_empty(tree))
		{
			return os;
		}
		return print(os, tree, max_leaf_count, 0u, tree.root) << std::endl;
	}
}
