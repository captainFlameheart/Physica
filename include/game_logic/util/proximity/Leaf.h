#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_logic::util::proximity
{
	struct Leaf
	{
		GLuint contact;
		Bounding_Box bounding_box;
		union
		{
			GLuint change;
			GLuint next_free;
		};
		// Leaf -> Change Index
		// Leaf -> (Leaf, Contact) -> (Leaf, Contact)
		// 0 -> (1, 32) -> (3, 47)
		// 1 -> (0, 32) -> (2, 35)
	};
}
