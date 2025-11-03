#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::rigid_bodies::velocities
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
