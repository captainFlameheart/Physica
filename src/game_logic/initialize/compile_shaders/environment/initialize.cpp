#include "game_logic/initialize/compile_shaders/environment/include.h"
#include "game_environment/environment.h"
#include "game_logic/shader_group/include.h"
#include "game_state/device_requirements/OpenGL_versions.h"
#include "util/shader/shader.h"
#include "game_state/bindings/include.h"
#include "game_state/local_sizes/include.h"
#include "game_state/vertex_factors/include.h"
#include "game_state/entity_type_indices/entity_type_indices.h"
#include "game_state/shader_to_entity_type/shader_to_entity_type.h"
#include "game_state/units/include.h"
#include <iostream>

namespace game_logic::initialize::compile_shaders::environment
{
	void initialize
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment& compile_environment
	)
	{
		::game_logic::shader_group::initialize(compile_environment.shader_group);

		compile_environment.version_directive = "#version " + std::string{ ::game_state::device_requirements::OpenGL_versions::version_string } + '\n';
		
		compile_environment.readonly_constant_definitions =
			"#define FIXED_DATA_RESTRICT_PLACEHOLDER \n"
			"#define FIXED_DATA_BUFFER_PLACEHOLDER uniform\n"
			"#define READ_ONLY_PLACEHOLDER readonly\n"
			"const uint fixed_data_binding = " + std::to_string(::game_state::bindings::uniform::fixed_data) + ";\n"
		;
		compile_environment.writable_constant_definitions =
			"#define FIXED_DATA_RESTRICT_PLACEHOLDER restrict\n"
			"#define FIXED_DATA_BUFFER_PLACEHOLDER buffer\n"
			"#define READ_ONLY_PLACEHOLDER \n"
			"const uint fixed_data_binding = " + std::to_string(::game_state::bindings::shader_storage::fixed_data) + ";\n"
		;

		constexpr GLuint dispatch_program_count
		{
			::game_state::shader_indices::tick::process_entities::count
			- ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::commit_counts::count
		};
		
		// TODO: Remove commit count programs.
		constexpr GLuint constraint_spawner_plan_compaction_program_base
		{
			::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::plan_compaction::base
			- ::game_state::shader_indices::tick::process_entities::base
		};
		constexpr GLuint constraint_spawner_clear_deaths_program_base
		{
			::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::base
			- ::game_state::shader_indices::tick::process_entities::base
		};

		/*constexpr GLuint constraint_spawner_plan_compaction_program_base
		{
			::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::plan_compaction::base
			- ::game_state::shader_indices::tick::process_entities::base
			- ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::commit_counts::count
		};*/

		constexpr GLuint constraint_spawner_perform_compaction_program_base
		{
			constraint_spawner_clear_deaths_program_base + ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::count
		};

		constexpr GLuint constraint_spawner_tick_entities_program_base
		{
			constraint_spawner_perform_compaction_program_base + ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::perform_compaction::count
		};

		constexpr GLuint constraint_plan_compaction_program_base
		{
			::game_state::shader_indices::tick::process_entities::pre_constraints::plan_compaction::base
			- ::game_state::shader_indices::tick::process_entities::base
		};

		constexpr GLuint constraint_clear_deaths_program_base
		{
			::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::base
			- ::game_state::shader_indices::tick::process_entities::base
		};

		constexpr GLuint constraint_perform_compaction_program_base
		{
			constraint_clear_deaths_program_base + ::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::count
		};

		constexpr GLuint constraint_tick_entities_program_base
		{
			::game_state::shader_indices::tick::process_entities::constraints::base - ::game_state::shader_indices::tick::process_entities::base
			//constraint_perform_compaction_program_base + ::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::count
		};

		std::string constraint_spawners_local_sizes_flags
		{
			"const uvec4 constraint_spawners_local_sizes_flags[" + std::to_string(::game_state::entity_type_indices::constraint_spawners::count) + "] = \n"
			"{	// (clear_deaths_local_size, perform_compaction_local_size, tick_entities_local_size, flags)\n"
		};
		for (GLuint i{ 0u }; i < ::game_state::entity_type_indices::constraint_spawners::count; ++i)
		{
			constexpr GLuint clear_deaths_base
			{
				::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::base
				- ::game_state::shader_indices::tick::process_entities::base
			};
			constexpr GLuint perform_compaction_base
			{
				::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::perform_compaction::base
				- ::game_state::shader_indices::tick::process_entities::base
			};
			constexpr GLuint tick_entities_base
			{
				::game_state::shader_indices::tick::process_entities::constraint_spawners::base
				- ::game_state::shader_indices::tick::process_entities::base
			};

			GLuint clear_deaths_local_size{ ::game_state::local_sizes::process_entities_local_sizes[clear_deaths_base + i] };
			GLuint perform_compaction_local_size{ ::game_state::local_sizes::process_entities_local_sizes[perform_compaction_base + i] };
			GLuint tick_entities_local_size{ ::game_state::local_sizes::process_entities_local_sizes[tick_entities_base + i] };
			
			constraint_spawners_local_sizes_flags += "	uvec4(" + 
				std::to_string(clear_deaths_local_size) + ", " + 
				std::to_string(perform_compaction_local_size) + ", " + 
				std::to_string(tick_entities_local_size) + ", 0u" + "),\n";
		}
		constraint_spawners_local_sizes_flags += "};\n";
		std::cout << constraint_spawners_local_sizes_flags << std::endl;
		
		std::string constraint_local_sizes_flags
		{
			"const uvec4 constraint_local_sizes_flags[" + std::to_string(::game_state::entity_type_indices::constraints::count) + "] = \n"
			"{	// (plan_compaction_local_size, perform_compaction_local_size, tick_entities_local_size, flags)\n"
		};
		for (GLuint i{ 0u }; i < ::game_state::entity_type_indices::constraints::count; ++i)
		{
			constexpr GLuint clear_deaths_base
			{
				::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::base
				- ::game_state::shader_indices::tick::process_entities::base
			};
			constexpr GLuint perform_compaction_base
			{
				::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::base
				- ::game_state::shader_indices::tick::process_entities::base
			};
			constexpr GLuint tick_entities_base
			{
				::game_state::shader_indices::tick::process_entities::constraints::base
				- ::game_state::shader_indices::tick::process_entities::base
			};

			GLuint clear_deaths_local_size{ ::game_state::local_sizes::process_entities_local_sizes[clear_deaths_base + i] };
			GLuint perform_compaction_local_size{ ::game_state::local_sizes::process_entities_local_sizes[perform_compaction_base + i] };
			GLuint tick_entities_local_size{ ::game_state::local_sizes::process_entities_local_sizes[tick_entities_base + i] };

			constraint_local_sizes_flags += "	uvec4(" +
				std::to_string(clear_deaths_local_size) + ", " +
				std::to_string(perform_compaction_local_size) + ", " +
				std::to_string(tick_entities_local_size) + ", 0u" + "),\n";
		}
		constraint_local_sizes_flags += "};\n";
		std::cout << constraint_local_sizes_flags << std::endl;

		std::string	dispatch_command_blueprints
		{
			"const uvec2 dispatch_command_blueprints[" + std::to_string(::game_state::shader_indices::tick::process_entities::count) + "] = \n"
			"{	// (entity_type_index, local_size)\n"
		};
		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::bodies::count; ++i)
		{
			GLuint entity_type{ static_cast<GLuint>(::game_state::shader_to_entity_type::tick_bodies_shader_to_entity_type[i]) };
			constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bodies::base - ::game_state::shader_indices::tick::process_entities::base };
			GLuint local_size{ ::game_state::local_sizes::process_entities_local_sizes[base + i] };
			dispatch_command_blueprints += "	uvec2(" + std::to_string(entity_type) + ", " + std::to_string(local_size) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::count; ++i)
		{
			GLuint entity_type{ static_cast<GLuint>(::game_state::shader_to_entity_type::tick_bounding_box_leaf_shader_to_entity_type[i]) };
			constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::base - ::game_state::shader_indices::tick::process_entities::base };
			GLuint local_size{ ::game_state::local_sizes::process_entities_local_sizes[base + i] };
			dispatch_command_blueprints += "	uvec2(" + std::to_string(entity_type) + ", " + std::to_string(local_size) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::swap_leaf_bounding_box_buffers::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		// TODO: REMOVE
		dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::plan_compaction::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::perform_compaction::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::constraint_spawners::count; ++i)
		{
			GLuint entity_type{ static_cast<GLuint>(::game_state::shader_to_entity_type::tick_constraint_spawners_shader_to_entity_type[i]) };
			constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::constraint_spawners::base - ::game_state::shader_indices::tick::process_entities::base };
			GLuint local_size{ ::game_state::local_sizes::process_entities_local_sizes[base + i] };
			dispatch_command_blueprints += "	uvec2(" + std::to_string(entity_type) + ", " + std::to_string(local_size) + "),\n";
		}

		// TODO: REMOVE
		dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::pre_constraints::plan_compaction::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::on_tick_constraints::count; ++i)
		{
			dispatch_command_blueprints += "	uvec2(" + std::to_string(0u) + ", " + std::to_string(0u) + "),\n";
		}

		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::constraints::count; ++i)
		{
			GLuint entity_type{ static_cast<GLuint>(::game_state::shader_to_entity_type::tick_constraints_shader_to_entity_type[i]) };
			constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::constraints::base - ::game_state::shader_indices::tick::process_entities::base };
			GLuint local_size{ ::game_state::local_sizes::process_entities_local_sizes[base + i] };
			dispatch_command_blueprints += "	uvec2(" + std::to_string(entity_type) + ", " + std::to_string(local_size) + "),\n";
		}
		/*for (GLuint dispatch_command_index{0u}; dispatch_command_index < ::game_state::shader_indices::tick::process_entities::count; ++dispatch_command_index)
		{
			GLuint entity_type_index{ static_cast<GLuint>(::game_state::shader_to_entity_type::shader_to_entity_type[dispatch_command_index]) };
			GLuint local_size{ ::game_state::local_sizes::process_entities_local_sizes[dispatch_command_index] };
			dispatch_command_blueprints += "	uvec2(" + std::to_string(entity_type_index) + ", " + std::to_string(local_size) + "),\n";
		}*/
		dispatch_command_blueprints += "};\n";

		std::cout << dispatch_command_blueprints << std::endl;

		std::string	draw_arrays_command_blueprints
		{
			"const uvec2 draw_arrays_command_blueprints[" + std::to_string(::game_state::shader_indices::draw::entities::count) + "] = \n"
			"{	// (entity_type_index, vertex_factor)\n"
		};
		for (GLuint draw_arrays_command_index{ 0u }; draw_arrays_command_index < ::game_state::shader_indices::draw::entities::count; ++draw_arrays_command_index)
		{
			GLuint entity_type_index{ static_cast<GLuint>(::game_state::shader_to_entity_type::draw_entities_shader_to_entity_type[draw_arrays_command_index]) };
			GLuint vertex_factor{ ::game_state::vertex_factors::draw_entities_vertex_factors[draw_arrays_command_index] };
			draw_arrays_command_blueprints += "	uvec2(" + std::to_string(entity_type_index) + ", " + std::to_string(vertex_factor) + "),\n";
		}
		draw_arrays_command_blueprints += "};\n";

		constexpr GLuint update_tick_counts_local_size{ ::game_state::local_sizes::update_tick_counts_local_size };
		constexpr GLuint update_draw_counts_local_size{ ::game_state::local_sizes::update_draw_counts_local_size };

		constexpr GLuint tick_entities_local_size_base{ ::game_state::shader_indices::tick::process_entities::base };

		constexpr GLuint process_point_masses_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses) - tick_entities_local_size_base
		] };
		constexpr GLuint tick_rigid_bodies_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::rigid_bodies) - tick_entities_local_size_base
		] };

		constexpr GLuint tick_rigid_body_circles_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::Indices::rigid_body_circles) - tick_entities_local_size_base
		] };

		constexpr GLuint swap_leaf_bounding_box_buffers_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::swap_leaf_bounding_box_buffers::Indices::swap_leaf_bounding_box_buffers) - tick_entities_local_size_base
		] };

		constexpr GLuint commit_constraint_spawner_counts_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::commit_counts::Indices::commit_counts) - tick_entities_local_size_base
		] };

		constexpr GLuint plan_rigid_body_circle_contact_constraint_spawner_compaction_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::plan_compaction::Indices::plan_rigid_body_circle_contact_constraint_spawner_compaction) - tick_entities_local_size_base
		] };

		constexpr GLuint clear_rigid_body_circle_contact_constraint_spawner_deaths_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::Indices::clear_rigid_body_circle_contact_constraint_spawner_deaths) - tick_entities_local_size_base
		] };

		constexpr GLuint perform_rigid_body_circle_contact_constraint_spawner_compaction_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::perform_compaction::Indices::perform_rigid_body_circle_contact_constraint_spawner_compaction) - tick_entities_local_size_base
		] };

		constexpr GLuint tick_rigid_body_circle_contact_constraint_spawners_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawners) - tick_entities_local_size_base
		] };

		constexpr GLuint commit_constraint_counts_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::commit_counts::Indices::commit_counts) - tick_entities_local_size_base
		] };

		constexpr GLuint plan_rigid_body_circle_contact_constraint_compaction_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::plan_compaction::Indices::plan_rigid_body_circle_contact_constraint_compaction) - tick_entities_local_size_base
		] };

		constexpr GLuint clear_rigid_body_circle_contact_constraint_deaths_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::Indices::clear_rigid_body_circle_contact_constraint_deaths) - tick_entities_local_size_base
		] };

		constexpr GLuint perform_rigid_body_circle_contact_constraint_compaction_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::pre_constraints::perform_compaction::Indices::perform_rigid_body_circle_contact_constraint_compaction) - tick_entities_local_size_base
		] };

		constexpr GLuint process_point_mass_distance_constraints_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_distance_constraints) - tick_entities_local_size_base
		] };

		constexpr GLuint tick_rigid_body_circle_contact_constraints_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::rigid_body_circle_contact_constraints) - tick_entities_local_size_base
		] };

		compile_environment.constant_definitions = 
			constraint_spawners_local_sizes_flags +
			constraint_local_sizes_flags +

			dispatch_command_blueprints +
			draw_arrays_command_blueprints +

			::util::shader::file_to_string("tick/pre_entities/commit_counts/commit_counts") + '\n' +
			::util::shader::file_to_string("tick/pre_entities/plan_compaction/plan_compaction") + '\n' +
			::util::shader::file_to_string("tick/pre_entities/clear_deaths/clear_deaths") + '\n' +
			::util::shader::file_to_string("tick/pre_entities/perform_compaction/determine_flow") + '\n' +

			initialize_input_constants(environment) +

			"const float meter_in_length_units = " + std::to_string(game_state::units::meter_in_length_units) + ";\n"
			"const float length_unit_in_meters = " + std::to_string(game_state::units::length_unit_in_meters) + ";\n"
			"const float radian_in_angle_units = " + std::to_string(game_state::units::radian_in_angle_units) + ";\n"
			"const float angle_unit_in_radians = " + std::to_string(game_state::units::angle_unit_in_radians) + ";\n"
			"const float second_in_time_units = " + std::to_string(game_state::units::second_in_time_units) + ";\n"
			"const float time_unit_in_seconds = " + std::to_string(game_state::units::time_unit_in_seconds) + ";\n"
			"const float meters_per_second_in_length_units_per_time_unit = " + std::to_string(game_state::units::meters_per_second_in_length_units_per_time_unit) + ";\n"
			"const float meters_per_second_squared_in_length_units_per_time_unit_squared = " + std::to_string(game_state::units::meters_per_second_squared_in_length_units_per_time_unit_squared) + ";\n"
			"const float radians_per_second_in_angle_units_per_time_unit = " + std::to_string(game_state::units::radians_per_second_in_angle_units_per_time_unit) + ";\n"
			"const float radians_per_second_squared_in_angle_units_per_time_unit_squared = " + std::to_string(game_state::units::radians_per_second_squared_in_angle_units_per_time_unit_squared) + ";\n"

			"const uint entity_type_count = " + std::to_string(game_state::entity_type_indices::count) + ";\n"
			"const uint constraint_spawner_type_base = " + std::to_string(game_state::entity_type_indices::constraint_spawners::base) + ";\n"
			"const uint constraint_spawner_type_count = " + std::to_string(game_state::entity_type_indices::constraint_spawners::count) + ";\n"
			"const uint constraint_type_base = " + std::to_string(game_state::entity_type_indices::constraints::base) + ";\n"
			"const uint constraint_type_count = " + std::to_string(game_state::entity_type_indices::constraints::count) + ";\n"

			"const uint leaf_bounding_box_type_count = " + std::to_string(game_state::leaf_bounding_box_types::count) + ";\n"

			"const uint dispatch_program_count = " + std::to_string(::game_state::shader_indices::tick::process_entities::count) + ";\n"
			"const uint draw_arrays_program_count = " + std::to_string(::game_state::shader_indices::draw::entities::count) + ";\n"

			"const uint constraint_spawner_plan_compaction_program_base = " + std::to_string(constraint_spawner_plan_compaction_program_base) + ";\n"
			"const uint constraint_spawner_clear_deaths_program_base = " + std::to_string(constraint_spawner_clear_deaths_program_base) + ";\n"
			"const uint constraint_spawner_perform_compaction_program_base = " + std::to_string(constraint_spawner_perform_compaction_program_base) + ";\n"
			"const uint constraint_spawner_tick_entities_program_base = " + std::to_string(constraint_spawner_tick_entities_program_base) + ";\n"
			"const uint constraint_clear_deaths_program_base = " + std::to_string(constraint_clear_deaths_program_base) + ";\n"
			"const uint constraint_perform_compaction_program_base = " + std::to_string(constraint_perform_compaction_program_base) + ";\n"
			"const uint constraint_tick_entities_program_base = " + std::to_string(constraint_tick_entities_program_base) + ";\n"

			"const uint point_mass_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::bodies::Indices::point_mass)) + ";\n"
			"const uint rigid_body_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::bodies::Indices::rigid_body)) + ";\n"
			"const uint rigid_body_triangle_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::body_attachments::Indices::rigid_body_triangle)) + ";\n"
			"const uint rigid_body_circle_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::body_attachments::Indices::rigid_body_circle)) + ";\n"
			"const uint inner_bounding_box_type = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::bounding_volume_hierarchy::Indices::inner_bounding_box)) + ";\n"
			"const uint rigid_body_circle_contact_constraint_spawner_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawner)) + ";\n"
			"const uint point_mass_distance_constraint_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::constraints::Indices::point_mass_distance_constraint)) + ";\n"
			"const uint rigid_body_circle_contact_constraint_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::constraints::Indices::rigid_body_circle_contact_constraint)) + ";\n"

			"const uint rigid_body_triangle_bounding_box_type = " + std::to_string(static_cast<GLuint>(::game_state::leaf_bounding_box_types::Indices::rigid_body_triangle)) + ";\n"
			"const uint rigid_body_circle_bounding_box_type = " + std::to_string(static_cast<GLuint>(::game_state::leaf_bounding_box_types::Indices::rigid_body_circle)) + ";\n"

			"const uint uvec4_data_binding = " + std::to_string(::game_state::bindings::shader_storage::uvec4_data) + ";\n"
			"const uint uvec2_data_binding = " + std::to_string(::game_state::bindings::shader_storage::uvec2_data) + ";\n"
			"const uint uint_data_binding = " + ::std::to_string(::game_state::bindings::shader_storage::uint_data) + ";\n"
			"const uint vec4_data_binding = " + ::std::to_string(::game_state::bindings::shader_storage::vec4_data) + ";\n"
			"const uint vec2_data_binding = " + ::std::to_string(::game_state::bindings::shader_storage::vec2_data) + ";\n"
			"const uint float_data_binding = " + ::std::to_string(::game_state::bindings::shader_storage::float_data) + ";\n"
			"const uint commands_binding = " + ::std::to_string(::game_state::bindings::shader_storage::commands) + ";\n"
			"const uint private_input_binding = " + ::std::to_string(::game_state::bindings::uniform::private_input) + ";\n"
			
			"const uint update_tick_counts_local_size = " + ::std::to_string(update_tick_counts_local_size) + ";\n"
			"const uint process_point_masses_local_size = " + ::std::to_string(process_point_masses_local_size) + ";\n"
			"const uint tick_rigid_bodies_local_size = " + ::std::to_string(tick_rigid_bodies_local_size) + ";\n"
			"const uint swap_leaf_bounding_box_buffers_local_size = " + ::std::to_string(swap_leaf_bounding_box_buffers_local_size) + ";\n"
			"const uint tick_rigid_body_circles_local_size = " + ::std::to_string(tick_rigid_body_circles_local_size) + ";\n"
			"const uint commit_constraint_spawner_counts_local_size = " + ::std::to_string(commit_constraint_spawner_counts_local_size) + ";\n"
			"const uint plan_rigid_body_circle_contact_constraint_spawner_compaction_local_size = " + ::std::to_string(plan_rigid_body_circle_contact_constraint_spawner_compaction_local_size) + ";\n"
			"const uint clear_rigid_body_circle_contact_constraint_spawner_deaths_local_size = " + ::std::to_string(clear_rigid_body_circle_contact_constraint_spawner_deaths_local_size) + ";\n"
			"const uint perform_rigid_body_circle_contact_constraint_spawner_compaction_local_size = " + ::std::to_string(perform_rigid_body_circle_contact_constraint_spawner_compaction_local_size) + ";\n"
			"const uint tick_rigid_body_circle_contact_constraint_spawners_local_size = " + ::std::to_string(tick_rigid_body_circle_contact_constraint_spawners_local_size) + ";\n"
			"const uint commit_constraint_counts_local_size = " + ::std::to_string(commit_constraint_counts_local_size) + ";\n"
			"const uint plan_rigid_body_circle_contact_constraint_compaction_local_size = " + ::std::to_string(plan_rigid_body_circle_contact_constraint_compaction_local_size) + ";\n"
			"const uint clear_rigid_body_circle_contact_constraint_deaths_local_size = " + ::std::to_string(clear_rigid_body_circle_contact_constraint_deaths_local_size) + ";\n"
			"const uint perform_rigid_body_circle_contact_constraint_compaction_local_size = " + ::std::to_string(perform_rigid_body_circle_contact_constraint_compaction_local_size) + ";\n"
			"const uint tick_point_mass_distance_constraints_local_size = " + ::std::to_string(process_point_mass_distance_constraints_local_size) + ";\n"
			"const uint tick_rigid_body_circle_contact_constraints_local_size = " + ::std::to_string(tick_rigid_body_circle_contact_constraints_local_size) + ";\n"
			"const uint update_draw_counts_local_size = " + ::std::to_string(update_draw_counts_local_size) + ";\n"

			"const uint fixed_data_buffer_data_size = " + ::std::to_string(environment.state.layouts.fixed_data.block_state.buffer_data_size) + ";\n"
			"const uint uvec4_data_offset = " + std::to_string(environment.state.layouts.uvec4_data.state.offset) + ";\n"
			"const uint uvec4_data_array_stride = " + std::to_string(environment.state.layouts.uvec4_data.state.array_stride) + ";\n"
			"const uint uvec2_data_offset = " + std::to_string(environment.state.layouts.uvec2_data.state.offset) + ";\n"
			"const uint uvec2_data_array_stride = " + std::to_string(environment.state.layouts.uvec2_data.state.array_stride) + ";\n"
			"const uint uint_data_offset = " + std::to_string(environment.state.layouts.uint_data.state.offset) + ";\n"
			"const uint uint_data_array_stride = " + std::to_string(environment.state.layouts.uint_data.state.array_stride) + ";\n"
			"const uint vec4_data_offset = " + std::to_string(environment.state.layouts.vec4_data.state.offset) + ";\n"
			"const uint vec4_data_array_stride = " + std::to_string(environment.state.layouts.vec4_data.state.array_stride) + ";\n"
			"const uint vec2_data_offset = " + std::to_string(environment.state.layouts.vec2_data.state.offset) + ";\n"
			"const uint vec2_data_array_stride = " + std::to_string(environment.state.layouts.vec2_data.state.array_stride) + ";\n"
			"const uint float_data_offset = " + std::to_string(environment.state.layouts.float_data.state.offset) + ";\n"
			"const uint float_data_array_stride = " + std::to_string(environment.state.layouts.float_data.state.array_stride) + ";\n"
		;

		compile_environment.fixed_data_source = ::util::shader::file_to_string("blocks/Fixed_Data");
		compile_environment.uvec4_data_source = ::util::shader::file_to_string("blocks/shader_storage/uvec4_Data");
		compile_environment.uvec2_data_source = ::util::shader::file_to_string("blocks/shader_storage/uvec2_Data");
		compile_environment.uint_data_source = ::util::shader::file_to_string("blocks/shader_storage/uint_Data");
		compile_environment.vec4_data_source = ::util::shader::file_to_string("blocks/shader_storage/vec4_Data");
		compile_environment.vec2_data_source = ::util::shader::file_to_string("blocks/shader_storage/vec2_Data");
		compile_environment.float_data_source = ::util::shader::file_to_string("blocks/shader_storage/float_Data");
		compile_environment.commands_source = ::util::shader::file_to_string("blocks/shader_storage/Commands");

		compile_environment.combined_data_source =
			compile_environment.fixed_data_source +
			compile_environment.uvec4_data_source +
			compile_environment.uvec2_data_source +
			compile_environment.uint_data_source +
			compile_environment.vec4_data_source +
			compile_environment.vec2_data_source +
			compile_environment.float_data_source +
			compile_environment.commands_source;

		compile_environment.constants_and_data_source = compile_environment.constant_definitions + compile_environment.combined_data_source;

		compile_environment.readonly_prefix_source =
			compile_environment.version_directive +
			compile_environment.readonly_constant_definitions +
			compile_environment.constants_and_data_source;
		compile_environment.writable_prefix_source =
			compile_environment.version_directive +
			compile_environment.writable_constant_definitions +
			compile_environment.constants_and_data_source;
	}
}
