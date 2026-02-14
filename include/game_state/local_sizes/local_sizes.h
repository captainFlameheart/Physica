#pragma once
#include "glad_glfw.h"

namespace game_state::local_sizes
{
	constexpr GLuint default_local_size{ 256u };

	struct Local_Sizes
	{
		GLuint process_point_masses{ default_local_size };
		GLuint process_point_mass_distance_constraints{ default_local_size };
		GLuint process_point_mass_uniform_force_constraints{ default_local_size };
	};

	constexpr Local_Sizes local_sizes{};

	constexpr GLuint compute_program_count{ sizeof(Local_Sizes) / sizeof(GLuint) };
}