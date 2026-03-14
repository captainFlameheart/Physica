#pragma once

namespace game_state::buffers::GPU_only
{
	struct State
	{
		GLuint size;
		GLuint count;
		GLuint current;
		GLuint* buffers;

		GLuint command_buffer_size;
		GLuint command_buffer;
	};
}
