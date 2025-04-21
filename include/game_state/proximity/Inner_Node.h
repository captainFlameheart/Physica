#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_state::proximity
{
	struct Inner_Node
	{
		Bounding_Box bounding_box;
		GLuint children[2];	// Bit 0 = 0 => Inner_Node, Bit 0 = 1 => Leaf
		GLuint parent;
	};
} // 7
