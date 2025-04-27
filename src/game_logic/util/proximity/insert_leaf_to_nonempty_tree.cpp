#include "game_logic/util/proximity/insert_leaf_to_nonempty_tree.h"
#include "game_logic/util/proximity/wrap.h"
#include "game_logic/util/proximity/compute_cost.h"

namespace game_logic::util::proximity
{
	void insert_leaf_to_nonempty_tree
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

		// Find sibling
		// TODO: Check if optimizing perimiter is better than optimizing area
		// TODO: Check if depth is to be considered when computing costs
		// TODO: Consider saving child combined boxes and areas for next descent iteration
		// TODO: Make a multi-node-tree version with a do-while loop instead
		GLuint sibling_index = tree.root;
		while (sibling_index >= max_leaf_count)
		{
			game_state::proximity::Node& sibling{ tree.nodes[sibling_index] };
			game_state::proximity::Bounding_Box parent_box;
			wrap(leaf.bounding_box, sibling.bounding_box, parent_box);
			GLint const cost{ compute_cost(parent_box) };

			GLint const inherited_cost{ cost - compute_cost(sibling.bounding_box) };

			game_state::proximity::Node const& child_0{ tree.nodes[sibling.children[0]] };
			game_state::proximity::Bounding_Box parent_box_0;
			wrap(leaf.bounding_box, child_0.bounding_box, parent_box_0);
			GLint const cost_0{ inherited_cost + (compute_cost(parent_box_0) - compute_cost(child_0.bounding_box)) };

			game_state::proximity::Node const& child_1{ tree.nodes[sibling.children[1]] };
			game_state::proximity::Bounding_Box parent_box_1;
			wrap(leaf.bounding_box, child_1.bounding_box, parent_box_1);
			GLint const cost_1{ inherited_cost + (compute_cost(parent_box_1) - compute_cost(child_1.bounding_box)) };

			if (cost < cost_0 && cost < cost_1)
			{
				break;
			}

			sibling.bounding_box = parent_box;
			sibling_index = cost_0 < cost_1 ? sibling.children[0] : sibling.children[1];
		}

		// Pair leaf with sibling
		GLuint parent_index{ tree.next_free_parent_node };
		tree.next_free_parent_node = tree.nodes[parent_index].next_free;

		game_state::proximity::Node& parent{ tree.nodes[parent_index] };
		parent.children[0] = leaf_index;
		parent.children[1] = sibling_index;
		game_state::proximity::Node& sibling{ tree.nodes[sibling_index] };
		wrap(leaf.bounding_box, sibling.bounding_box, parent.bounding_box);
		parent.parent = sibling.parent;
		leaf.parent = parent_index;
		sibling.parent = parent_index;
		if (tree.root == sibling_index)
		{
			tree.root = parent_index;
		}
		else
		{
			game_state::proximity::Node& grandparent{ tree.nodes[parent.parent] };
			GLuint const parent_side{ static_cast<GLuint>(grandparent.children[1] == sibling_index) };
			grandparent.children[parent_side] = parent_index;
		}
	}
}
