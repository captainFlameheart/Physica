#pragma once
#include "glad_glfw.h"
#include "game_state/shader_indices/include.h"
#include "game_state/entity_type_indices/include.h"
#include <array>

namespace game_state::shader_to_entity_type
{
	/*constexpr GLuint entity_shader_count
	{
		::game_state::shader_indices::tick::process_entities::count + ::game_state::shader_indices::draw::entities::count
	};*/

	constexpr std::array<::game_state::entity_type_indices::bodies::Indices, ::game_state::shader_indices::tick::process_entities::bodies::count>
	initialize_tick_bodies_shader_to_entity_type()
	{
		std::array<::game_state::entity_type_indices::bodies::Indices, ::game_state::shader_indices::tick::process_entities::bodies::count>
			shader_to_entity_type{};

		constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bodies::base };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses) - base] =
			::game_state::entity_type_indices::bodies::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::rigid_bodies) - base] =
			::game_state::entity_type_indices::bodies::Indices::rigid_body;

		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::skycircle_elements::Indices, ::game_state::shader_indices::tick::process_entities::skycircle_elements::count>
		initialize_tick_skycircle_elements_shader_to_entity_type()
	{
		std::array<::game_state::entity_type_indices::skycircle_elements::Indices, ::game_state::shader_indices::tick::process_entities::skycircle_elements::count>
			shader_to_entity_type{};

		constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::skycircle_elements::base };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::skycircle_elements::Indices::default_elements) - base] =
			::game_state::entity_type_indices::skycircle_elements::Indices::default_skycircle_element;

		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::body_attachments::Indices, ::game_state::shader_indices::tick::process_entities::bodies::count>
		initialize_tick_bounding_box_leaf_shader_to_entity_type()
	{
		std::array<::game_state::entity_type_indices::body_attachments::Indices, ::game_state::shader_indices::tick::process_entities::bodies::count>
			shader_to_entity_type{};

		constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::base };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::Indices::rigid_body_circles) - base] =
			::game_state::entity_type_indices::body_attachments::Indices::rigid_body_circle;

		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::bounding_volume_hierarchy::Indices, ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::count>
		initialize_tick_bounding_volume_hierarchy_shader_to_entity_type()
	{
		std::array<::game_state::entity_type_indices::bounding_volume_hierarchy::Indices, ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::count>
			shader_to_entity_type{};

		constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::base };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::Indices::inner_bounding_boxes) - base] =
			::game_state::entity_type_indices::bounding_volume_hierarchy::Indices::inner_bounding_box;

		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::constraint_spawners::Indices, ::game_state::shader_indices::tick::process_entities::constraint_spawners::count>
		initialize_tick_constraint_spawners_shader_to_entity_type()
	{
		std::array<::game_state::entity_type_indices::constraint_spawners::Indices, ::game_state::shader_indices::tick::process_entities::constraint_spawners::count>
			shader_to_entity_type{};

		constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::constraint_spawners::base };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraint_spawners::Indices::bounding_box_contact_detectors) - base] =
			::game_state::entity_type_indices::constraint_spawners::Indices::bounding_box_contact_detector;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawners) - base] =
			::game_state::entity_type_indices::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawner;

		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::constraints::Indices, ::game_state::shader_indices::tick::process_entities::constraints::count>
		initialize_tick_constraints_shader_to_entity_type()
	{
		std::array<::game_state::entity_type_indices::constraints::Indices, ::game_state::shader_indices::tick::process_entities::constraints::count>
			shader_to_entity_type{};

		constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::constraints::base };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_distance_constraints) - base] =
			::game_state::entity_type_indices::constraints::Indices::point_mass_distance_constraint;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::rigid_body_circle_contact_constraints) - base] =
			::game_state::entity_type_indices::constraints::Indices::rigid_body_circle_contact_constraint;

		return shader_to_entity_type;
	}

	constexpr std::array<GLuint, ::game_state::shader_indices::draw::entities::count>
		initialize_draw_entities_shader_to_entity_type()
	{
		std::array<GLuint, ::game_state::shader_indices::draw::entities::count>
			shader_to_entity_type{};

		constexpr GLuint base{ ::game_state::shader_indices::draw::entities::base };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_triangles) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::body_attachments::Indices::rigid_body_triangle);
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_circles) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::body_attachments::Indices::rigid_body_circle);

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_masses) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::bodies::Indices::point_mass);
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_velocities) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::bodies::Indices::point_mass);
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_forces) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::bodies::Indices::point_mass);
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_bodies) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::bodies::Indices::rigid_body);

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraint_spawners::Indices::bounding_box_contact_detectors) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::constraint_spawners::Indices::bounding_box_contact_detector);
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawners) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawner);

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::constraints::Indices::point_mass_distance_constraint);

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bounding_volume_hierarchy::Indices::rigid_body_circle_bounding_boxes) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::body_attachments::Indices::rigid_body_circle);
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bounding_volume_hierarchy::Indices::inner_bounding_boxes) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::bounding_volume_hierarchy::Indices::inner_bounding_box);

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::skycircle_elements::Indices::default_elements) - base] =
			static_cast<GLuint>(::game_state::entity_type_indices::skycircle_elements::Indices::default_skycircle_element);

		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::bodies::Indices, ::game_state::shader_indices::tick::process_entities::bodies::count>
		tick_bodies_shader_to_entity_type{ initialize_tick_bodies_shader_to_entity_type() };

	constexpr std::array<::game_state::entity_type_indices::skycircle_elements::Indices, ::game_state::shader_indices::tick::process_entities::skycircle_elements::count>
		tick_skycircle_elements_shader_to_entity_type{ initialize_tick_skycircle_elements_shader_to_entity_type() };
	
	constexpr std::array<::game_state::entity_type_indices::body_attachments::Indices, ::game_state::shader_indices::tick::process_entities::bodies::count>
		tick_bounding_box_leaf_shader_to_entity_type{ initialize_tick_bounding_box_leaf_shader_to_entity_type() };

	constexpr std::array<::game_state::entity_type_indices::bounding_volume_hierarchy::Indices, ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::count>
		tick_bounding_volume_hierarchy_shader_to_entity_type{ initialize_tick_bounding_volume_hierarchy_shader_to_entity_type() };

	constexpr std::array<::game_state::entity_type_indices::constraint_spawners::Indices, ::game_state::shader_indices::tick::process_entities::constraint_spawners::count>
		tick_constraint_spawners_shader_to_entity_type{ initialize_tick_constraint_spawners_shader_to_entity_type() };

	constexpr std::array<::game_state::entity_type_indices::constraints::Indices, ::game_state::shader_indices::tick::process_entities::constraints::count>
		tick_constraints_shader_to_entity_type{ initialize_tick_constraints_shader_to_entity_type() };

	constexpr std::array<GLuint, ::game_state::shader_indices::draw::entities::count>
		draw_entities_shader_to_entity_type{ initialize_draw_entities_shader_to_entity_type() };

	/*constexpr std::array<GLuint, entity_shader_count> initialize_shader_to_entity_type()
	{
		std::array<GLuint, entity_shader_count> shader_to_entity_type{};

		constexpr GLuint tick_entities_base{ ::game_state::shader_indices::tick::process_entities::base };
		constexpr GLuint draw_entities_base{ ::game_state::shader_indices::draw::entities::base - ::game_state::shader_indices::tick::process_entities::count };

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses) - tick_entities_base] = 
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::rigid_bodies) - tick_entities_base] = 
			::game_state::entity_type_indices::Indices::rigid_body;
		
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawners) - tick_entities_base] =
			::game_state::entity_type_indices::Indices::rigid_body_circle_contact_constraint_spawner;
		
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_distance_constraints) - tick_entities_base] = 
			::game_state::entity_type_indices::Indices::point_mass_distance_constraint;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_uniform_force_constraints) - tick_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass_uniform_force_constraint;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::rigid_body_circle_contact_constraints) - tick_entities_base] =
			::game_state::entity_type_indices::Indices::rigid_body_circle_contact_constraint;

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_masses) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_velocities) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::point_mass_forces) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_triangles) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::rigid_body_triangle;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_body_circles) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::rigid_body_circle;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::bodies::Indices::rigid_bodies) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::rigid_body;

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawners) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::rigid_body_circle_contact_constraint_spawner;

		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_distance_constraints) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass_distance_constraint;
		shader_to_entity_type[static_cast<GLuint>(::game_state::shader_indices::draw::entities::constraints::Indices::point_mass_uniform_force_constraints) - draw_entities_base] =
			::game_state::entity_type_indices::Indices::point_mass_uniform_force_constraint;
		
		return shader_to_entity_type;
	}

	constexpr std::array<::game_state::entity_type_indices::Indices, entity_shader_count> shader_to_entity_type{ initialize_shader_to_entity_type() };
	*/
}
