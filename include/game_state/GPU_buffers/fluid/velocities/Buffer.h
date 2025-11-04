#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid::velocities
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint v_offset;
		GLint v_stride;

		GLuint snapshot_buffer;
	};
}
