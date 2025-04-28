#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"
#include "game_logic/util/proximity/NULL_INDEX.h"

namespace game_logic::util::proximity
{
	template <typename On_Leaf_Found>
	void query_region_of_multinode_tree
	(
		game_state::proximity::Tree& tree, GLuint const leaf_count, 
		game_state::proximity::Bounding_Box const& region, 
		On_Leaf_Found& on_leaf_found
	)
	{
		game_state::proximity::Node const& root{ tree.nodes[tree.root] };
		if (overlaps(region, root.bounding_box))
		{
			tree.to_visit[0] = root.child_0;
			tree.to_visit[1] = root.child_1;
			GLuint next_to_visit{ 1u };

			do
			{
				GLuint const node_index{ tree.to_visit[next_to_visit--] };
				game_state::proximity::Node const& node{ tree.nodes[node_index] };
				if (overlaps(region, node.bounding_box))
				{
					if (node_index < leaf_count)
					{
						on_leaf_found(node_index, node);
					}
					else
					{
						tree.to_visit[++next_to_visit] = node.child_0;
						tree.to_visit[++next_to_visit] = node.child_1;
					}
				}
			} while (next_to_visit != game_logic__util__proximity_NULL_INDEX);
		}		
	}
}
