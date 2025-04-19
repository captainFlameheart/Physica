#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_logic::util::proximity
{
	struct Contact
	{
		GLuint leaf;
		GLuint GPU_index;
		GLuint next;
	};
}
