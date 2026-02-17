#pragma once
#include "glad_glfw.h"
#include "game_state/shader_indices/include.h"
#include "game_state/entity_type_indices/include.h"
#include <array>

namespace game_state::shader_to_entity_type
{
	constexpr GLuint entity_shader_count
	{
		::game_state::shader_indices::tick::process_entities::count + ::game_state::shader_indices::draw::entities::count
	};

	constexpr std::array<::game_state::entity_type_indices::Indices, entity_shader_count> initialize_shader_to_entity_type()
	{
		std::array<::game_state::entity_type_indices::Indices, entity_shader_count> shader_to_entity_type{};

		constexpr GLuint tick_entities_base{ ::game_state::shader_indices::tick::process_entities::base };
		constexpr GLuint draw_entities_base{ ::game_state::shader_indices::draw::entities::base - ::game_state::shader_indices::tick::process_entities::count };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses) - tick_entities_base] = 
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::rigid_bodies) - tick_entities_base] = 
			::game_state::entity_type_indices::Indices::rigid_body;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_distance_constraints) - tick_entities_base] = 
			::game_state::entity_type_indices::Indices::point_mass_distance_constraint;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_uniform_force_constraints) - tick_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass_uniform_force_constraint;

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_masses) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_velocities) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_forces) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_bodies) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::rigid_body;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass_distance_constraint;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_uniform_force_constraints) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass_uniform_force_constraint;
		
		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::Indices, entity_shader_count> shader_to_entity_type{ initialize_shader_to_entity_type() };
}
