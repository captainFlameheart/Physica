#pragma once

namespace game_state::buffers::GPU_only
{
	struct State
	{
		GLuint size;
		GLuint count;
		GLuint* buffers;
	};
}
