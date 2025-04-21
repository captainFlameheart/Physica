#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_state::proximity
{
	struct Leaf
	{
		Bounding_Box bounding_box;
		GLuint parent;
		GLuint contact;
		union
		{
			GLuint change;
			GLuint next_free;
		};
	};
} // 7
