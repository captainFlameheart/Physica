#pragma once
#include "glad_glfw.h"
#include "Bounding_Box.h"

namespace game_state::proximity
{
	struct Contact
	{
		GLuint leaf;
		GLuint data;
		GLuint next;
	};
}
