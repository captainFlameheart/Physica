#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_logic::util::proximity
{
	struct Leaf_Change
	{
		GLuint leaf;
		Bounding_Box from;
	};
}
