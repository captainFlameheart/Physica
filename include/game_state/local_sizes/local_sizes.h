#pragma once
#include "glad_glfw.h"
#include "game_state/shader_indices/include.h"
#include <array>

namespace game_state::local_sizes
{
	constexpr GLuint default_local_size{ 256u };

	constexpr GLuint update_tick_counts_local_size{ default_local_size };
	constexpr GLuint update_draw_counts_local_size{ default_local_size };

	constexpr std::array<GLuint, ::game_state::shader_indices::tick::process_entities::count> initialize_process_entities_local_sizes()
	{
		std::array<GLuint, ::game_state::shader_indices::tick::process_entities::count> local_sizes{};
		constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::base };
		local_sizes[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses) - base] = default_local_size;
		local_sizes[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::rigid_bodies) - base] = default_local_size;
		local_sizes[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_distance_constraints) - base] = default_local_size;
		local_sizes[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_uniform_force_constraints) - base] = default_local_size;
		return local_sizes;
	}

	constexpr std::array<GLuint, ::game_state::shader_indices::tick::process_entities::count> process_entities_local_sizes{ initialize_process_entities_local_sizes() };
}
