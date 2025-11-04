#pragma once

namespace game_state::GPU_buffers::cursor::constraint
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint offset_offset;
		GLint target_velocity_offset;
		GLint mass_offset;
		GLint mass_matrix_stride;
		GLint impulse_offset;
	};
}
