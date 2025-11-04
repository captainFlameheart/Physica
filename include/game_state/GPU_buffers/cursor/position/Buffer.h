#pragma once

namespace game_state::GPU_buffers::cursor::position
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint position_offset;
	};
}
