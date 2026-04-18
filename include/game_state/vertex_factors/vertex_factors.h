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
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_forces) - base] = 2u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_triangles) - base] = 3u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_circles) - base] = 6u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_bodies) - base] = 12u;

		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraint_spawners::Indices::bounding_box_contact_detectors) - base] = 6u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawners) - base] = 6u;

		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints) - base] = 6u;

		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bounding_volume_hierarchy::Indices::rigid_body_circle_bounding_boxes) - base] = 6u;
		vertex_factors[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bounding_volume_hierarchy::Indices::inner_bounding_boxes) - base] = 6u;

		return vertex_factors;
	}

	constexpr std::array<GLuint, ::game_state::shader_indices::draw::entities::count> draw_entities_vertex_factors{ initialize_draw_entities_vertex_factors() };
}
