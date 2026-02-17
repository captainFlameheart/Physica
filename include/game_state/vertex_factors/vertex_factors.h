#pragma once
#include "glad_glfw.h"
#include "game_state/shader_indices/include.h"
#include <array>

namespace game_state::vertex_factors
{
	constexpr std::array<GLuint, ::game_state::shader_indices::draw::entities::count> initialize_draw_entities_vertex_factors()
	{
		std::array<GLuint, ::game_state::shader_indices::draw::entities::count> vertex_factors{};
		constexpr GLuint base{ ::game_state::shader_indices::draw::entities::base };
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_masses) - base] = 6u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_velocities) - base] = 2u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_bodies) - base] = 12u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints) - base] = 6u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_uniform_force_constraints) - base] = 6u;
		return vertex_factors;
	}

	constexpr std::array<GLuint, ::game_state::shader_indices::draw::entities::count> draw_entities_vertex_factors{ initialize_draw_entities_vertex_factors() };
}
