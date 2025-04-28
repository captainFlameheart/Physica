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

		// TODO: Consider using some sort of dynamic memory pool so other processes 
		// can use this memory for their own needs. Placing memory on stack is an 
		// alternative, but that would require max leaf count to be a template argument 
		// so it would not be able to be changed at run-time (which we probably don't 
		// want to do either way since the leafs are mapped to GPU memory which 
		// is harder to make dynamic in size).
		GLuint* nodes_to_visit;
	};
}
