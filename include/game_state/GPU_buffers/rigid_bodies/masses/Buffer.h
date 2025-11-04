#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::rigid_bodies::masses
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint masses_offset;
		GLint masses_stride;
	};
}
