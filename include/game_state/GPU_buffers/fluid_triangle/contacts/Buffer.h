#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::fluid_triangle::contacts
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint contacts_offset;
		GLint contacts_stride;
		GLint contacts_triangle_offset;
		GLint contacts_mass_offset;
		GLint contacts_impulse_offset;
		GLint contacts_particle_offset;
		GLint contacts_body_offset;
		GLint contacts_target_velocity_offset;
		GLint contacts_offset_offset;
		GLint contacts_direction_offset;
		GLint contacts_impulse_range_offset;
	};
}
