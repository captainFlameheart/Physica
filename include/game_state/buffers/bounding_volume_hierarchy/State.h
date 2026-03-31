#pragma once
#include "glad_glfw.h"

namespace game_state::buffers::bounding_volume_hierarchy
{
	struct State
	{
		GLuint buffer;
		GLbyte* mapping;

		GLsync fence;
	};
}
