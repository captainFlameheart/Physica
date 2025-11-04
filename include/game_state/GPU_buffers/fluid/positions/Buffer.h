#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid::positions
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint p_offset;
		GLint p_stride;
	};
}
