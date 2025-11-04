#pragma once
#include <GLFW/glfw3.h>

namespace game_state::GPU_buffers::rigid_bodies::distance_constraints
{
	struct Buffer
	{
		GLuint buffer;
		GLint size;
		GLint count_offset;
		GLint distance_constraints_offset;
		GLint distance_constraints_stride;
		GLint distance_constraints_bodies_offset;
		GLint distance_constraints_local_points_offset;
		GLint distance_constraints_local_points_stride;
		GLint distance_constraints_offsets_offset;
		GLint distance_constraints_offsets_stride;
		GLint distance_constraints_direction_offset;
		GLint distance_constraints_max_distance_offset;
		GLint distance_constraints_target_velocity_offset;
		GLint distance_constraints_mass_offset;
		GLint distance_constraints_impulse_offset;

		GLuint current_count;
	};
}
