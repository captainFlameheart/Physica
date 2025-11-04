#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid_triangle::contact_count
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint count_offset;
	};
}
