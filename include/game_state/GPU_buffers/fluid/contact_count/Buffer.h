#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid::contact_count
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		//GLint fluid_contact_count_buffer_persistent_count_offset;
		GLint count_offset;

		GLuint current_persistent_contact_count;
		GLuint current_contact_count;
	};
}
