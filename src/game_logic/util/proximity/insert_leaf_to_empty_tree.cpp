#include "game_logic/util/proximity/insert_leaf_to_empty_tree.h"
#include "game_logic/util/proximity/NULL_INDEX.h"

namespace game_logic::util::proximity
{
	void insert_leaf_to_empty_tree
	(
		game_state::proximity::Tree& tree, GLuint const max_leaf_count,
		GLuint const leaf_index,
		GLint const min_x, GLint const min_y, GLint const max_x, GLint const max_y
	)
	{
		game_state::proximity::Node& leaf{ tree.nodes[leaf_index] };

		// Set bounding box
		leaf.bounding_box.min.x = min_x;
		leaf.bounding_box.min.y = min_y;
		leaf.bounding_box.max.x = max_x;
		leaf.bounding_box.max.y = max_y;

		leaf.contact = game_logic__util__proximity_NULL_INDEX;

		tree.root = leaf_index;
	}
}
