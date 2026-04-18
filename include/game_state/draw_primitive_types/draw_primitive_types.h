#pragma once
#include "glad_glfw.h"
#include "game_state/shader_indices/include.h"
#include <array>

namespace game_state::draw_primitive_types
{
	constexpr GLenum default_primitive_type{ GL_TRIANGLES };

	constexpr std::array<GLenum, ::game_state::shader_indices::draw::entities::count> initialize_entities_primitive_types()
	{
		std::array<GLenum, ::game_state::shader_indices::draw::entities::count> primitive_types{};
		constexpr GLuint base{ ::game_state::shader_indices::draw::entities::base };
		
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_masses) - base] = default_primitive_type;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_velocities) - base] = GL_LINES;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_forces) - base] = GL_LINES;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_triangles) - base] = default_primitive_type;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_circles) - base] = default_primitive_type;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_bodies) - base] = default_primitive_type;
		
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraint_spawners::Indices::bounding_box_contact_detectors) - base] = default_primitive_type;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawners) - base] = default_primitive_type;

		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints) - base] = default_primitive_type;
		
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bounding_volume_hierarchy::Indices::rigid_body_circle_bounding_boxes) - base] = default_primitive_type;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bounding_volume_hierarchy::Indices::inner_bounding_boxes) - base] = default_primitive_type;

		return primitive_types;
	}

	constexpr std::array<GLenum, ::game_state::shader_indices::draw::entities::count> entities_primitive_types{ initialize_entities_primitive_types() };
}
