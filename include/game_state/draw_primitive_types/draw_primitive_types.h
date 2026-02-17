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
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_bodies) - base] = default_primitive_type;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints) - base] = default_primitive_type;
		primitive_types[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_uniform_force_constraints) - base] = default_primitive_type;
		return primitive_types;
	}

	constexpr std::array<GLenum, ::game_state::shader_indices::draw::entities::count> entities_primitive_types{ initialize_entities_primitive_types() };
}
