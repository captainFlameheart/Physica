#pragma once
#include "glad_glfw.h"
#include "Leaf_Change.h"
#include "Node.h"
#include "Contact.h"

namespace game_state::proximity
{
	struct Tree
	{
		GLuint next_free_parent_node;
		GLuint changed_leaf_count;
		Leaf_Change* leaf_changes;
		GLuint root;
		Node* nodes;
		Contact* contacts;
	};
}
