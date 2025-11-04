#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid::contacts
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint contacts_offset;
		GLint contacts_stride;
		GLint contacts_inactive_offset;
		GLint contacts_impulse_offset;
		GLint contacts_particles_offset;
		GLint contacts_direction_offset;
		GLint contacts_target_velocity_offset;
		GLint contacts_mass_offset;
		GLint contacts_impulse_range_offset;
	};
}
