#include "game_logic/util/proximity/initialize.h"
#include "game_logic/util/proximity/NULL_INDEX.h"

namespace game_logic::util::proximity
{
	void initialize(game_state::proximity::Tree& tree, GLuint const max_leaf_count)
	{
		tree.leaf_changes = new game_state::proximity::Leaf_Change[max_leaf_count];
		
		tree.root = game_logic__util__proximity_NULL_INDEX;
		GLuint const max_node_count{ 2u * max_leaf_count - 1u };
		tree.nodes = new game_state::proximity::Node[max_node_count];
		tree.next_free_parent_node = max_node_count - 1u;
		while (tree.next_free_parent_node > max_leaf_count)
		{
			GLuint previous{ tree.next_free_parent_node - 1 };
			tree.nodes[previous].next_free = tree.next_free_parent_node;
			tree.next_free_parent_node = previous;
		}
	}
}
