#include "game_logic/util/proximity/change_leaf_of_multinode_tree.h"
#include "game_logic/util/proximity/wrap.h"
#include "game_logic/util/proximity/compute_cost.h"

namespace game_logic::util::proximity
{
	void change_leaf_of_multinode_tree
	(
		game_state::proximity::Tree& tree, GLuint const max_leaf_count, 
		GLuint const leaf_index, GLuint const change_index, 
		GLint const min_x, GLint const min_y, GLint const max_x, GLint const max_y
	)
	{
		game_state::proximity::Node& leaf{ tree.nodes[leaf_index] };

		// Keep track of change
		game_state::proximity::Leaf_Change& leaf_change{ tree.leaf_changes[change_index] };
		leaf_change.leaf = leaf_index;
		leaf_change.from = leaf.bounding_box;

		// Change bounding box
		leaf.bounding_box.min.x = min_x;
		leaf.bounding_box.min.y = min_y;
		leaf.bounding_box.max.x = max_x;
		leaf.bounding_box.max.y = max_y;

		leaf.change = change_index;

		game_state::proximity::Node& parent{ tree.nodes[leaf.parent] };
		GLuint const sibling_side{ static_cast<GLuint>(parent.children[0] == leaf_index) };

		// Remove leaf
		GLuint sibling_index{ parent.children[sibling_side] };
		if (tree.root == leaf.parent)
		{
			tree.root = sibling_index;
		}
		else
		{
			game_state::proximity::Node& grandparent{ tree.nodes[parent.parent] };
			GLuint const parent_side{ static_cast<GLuint>(grandparent.children[1] == leaf.parent) };
			grandparent.children[parent_side] = sibling_index;
			wrap
			(
				tree.nodes[grandparent.children[0]].bounding_box,
				tree.nodes[grandparent.children[1]].bounding_box,
				grandparent.bounding_box
			);
		}
		tree.nodes[sibling_index].parent = parent.parent;
		
		// Find new sibling
		// TODO: Check if optimizing perimiter is better than optimizing area
		// TODO: Check if depth is to be considered when computing costs
		// TODO: Consider saving child combined boxes and areas for next descent iteration
		sibling_index = tree.root;
		do
		{
			game_state::proximity::Node& sibling{ tree.nodes[sibling_index] };
			game_state::proximity::Bounding_Box parent_box;
			wrap(leaf.bounding_box, sibling.bounding_box, parent_box);
			GLint const cost{ compute_cost(parent_box) };

			GLint const inherited_cost{ cost - compute_cost(sibling.bounding_box) };
			
			game_state::proximity::Node const& child_0{tree.nodes[sibling.children[0]]};
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
		} while (sibling_index >= max_leaf_count);

		// Pair leaf with new sibling
		parent.children[sibling_side] = sibling_index;
		game_state::proximity::Node& sibling{ tree.nodes[sibling_index] };
		wrap(leaf.bounding_box, sibling.bounding_box, parent.bounding_box);
		parent.parent = sibling.parent;
		sibling.parent = leaf.parent;
		if (tree.root == sibling_index)
		{
			tree.root = leaf.parent;
		}
		else
		{
			game_state::proximity::Node& grandparent{ tree.nodes[parent.parent] };
			GLuint const parent_side{ static_cast<GLuint>(grandparent.children[1] == sibling_index) };
			grandparent.children[parent_side] = leaf.parent;
		}
	}
}
