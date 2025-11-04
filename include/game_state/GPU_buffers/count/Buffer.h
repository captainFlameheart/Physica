#pragma once

namespace game_state::GPU_buffers::count
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint bodies_offset;
		GLint triangles_offset;
		GLint fluid_particles_offset;
	};
}
