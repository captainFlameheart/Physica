#pragma once

namespace game_state::GPU_buffers::cursor::constrained_point
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint body_offset;
		GLint local_point_offset;
	};
}
