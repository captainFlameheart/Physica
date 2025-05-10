#pragma once
#include "glad_glfw.h"
#include "game_state/proximity/Tree.h"
#include "game_logic/util/proximity/NULL_INDEX.h"
#include "game_logic/util/proximity/contains_point.h"

namespace game_logic::util::proximity
{
	template <typename On_Leaf_Found>
	void query_point_of_multinode_tree
	(
		game_state::proximity::Tree& tree, GLuint const leaf_count,
		GLint const x, GLint const y,
		On_Leaf_Found& on_leaf_found
	)
	{
		game_state::proximity::Node const& root{ tree.nodes[tree.root] };
		if (contains_point(root.bounding_box, x, y))
		{
			tree.nodes_to_visit[0u] = root.child_0;
			tree.nodes_to_visit[1u] = root.child_1;
			GLuint next_to_visit{ 1u };

			do
			{
				GLuint const node_index{ tree.nodes_to_visit[next_to_visit--] };
				game_state::proximity::Node const& node{ tree.nodes[node_index] };
				if (contains_point(node.bounding_box, x, y))
				{
					if (node_index < leaf_count)
					{
						on_leaf_found(node_index, node);
					}
					else
					{
						tree.nodes_to_visit[++next_to_visit] = node.child_0;
						tree.nodes_to_visit[++next_to_visit] = node.child_1;
					}
				}
			} while (next_to_visit != game_logic__util__proximity_NULL_INDEX);
		}
	}
}
