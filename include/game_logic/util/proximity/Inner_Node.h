#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_logic::util::proximity
{
	struct Inner_Node
	{
		Bounding_Box bounding_box;
		GLuint child_0;	// Bit 0: 0 -> Inner_Node, 1 -> Leaf
		GLuint child_1;	// Bit 0: 0 -> Inner_Node, 1 -> Leaf
	};
}
