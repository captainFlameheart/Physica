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

		constexpr GLuint draw_arrays_program_count = 3u;

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
		
		std::string	dispatch_command_blueprints
		{
			"const uvec2 dispatch_command_blueprints[" + std::to_string(::game_state::shader_indices::tick::process_entities::count) + "] = \n"
			"{	// (entity_type_index, local_size)\n"
		};
		for (GLuint dispatch_command_index{ 0u }; dispatch_command_index < ::game_state::shader_indices::tick::process_entities::count; ++dispatch_command_index)
		{
			GLuint entity_type_index{ static_cast<GLuint>(::game_state::shader_to_entity_type::shader_to_entity_type[dispatch_command_index]) };
			GLuint local_size{ ::game_state::local_sizes::process_entities_local_sizes[dispatch_command_index] };
			dispatch_command_blueprints += "	uvec2(" + std::to_string(entity_type_index) + ", " + std::to_string(local_size) + "),\n";
		}
		dispatch_command_blueprints += "};\n";

		std::string	draw_arrays_command_blueprints
		{
			"const uvec2 draw_arrays_command_blueprints[" + std::to_string(::game_state::shader_indices::draw::entities::count) + "] = \n"
			"{	// (entity_type_index, vertex_factor)\n"
		};
		for (GLuint draw_arrays_command_index{ 0u }; draw_arrays_command_index < ::game_state::shader_indices::draw::entities::count; ++draw_arrays_command_index)
		{
			GLuint entity_type_index{ static_cast<GLuint>(::game_state::shader_to_entity_type::shader_to_entity_type[::game_state::shader_indices::tick::process_entities::count + draw_arrays_command_index]) };
			GLuint vertex_factor{ ::game_state::vertex_factors::draw_entities_vertex_factors[draw_arrays_command_index] };
			draw_arrays_command_blueprints += "	uvec2(" + std::to_string(entity_type_index) + ", " + std::to_string(vertex_factor) + "),\n";
		}
		draw_arrays_command_blueprints += "};\n";

		GLuint update_tick_counts_local_size{ ::game_state::local_sizes::update_tick_counts_local_size };
		GLuint update_draw_counts_local_size{ ::game_state::local_sizes::update_draw_counts_local_size };

		constexpr GLuint tick_entities_local_size_base{ ::game_state::shader_indices::tick::process_entities::base };

		GLuint process_point_masses_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::point_masses) - tick_entities_local_size_base
		] };
		GLuint process_rigid_bodies_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::bodies::Indices::rigid_bodies) - tick_entities_local_size_base
		] };

		GLuint process_point_mass_distance_constraints_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_distance_constraints) - tick_entities_local_size_base
		] };
		GLuint process_point_mass_uniform_force_constraints_local_size{ ::game_state::local_sizes::process_entities_local_sizes[
			static_cast<GLuint>(::game_state::shader_indices::tick::process_entities::constraints::Indices::point_mass_uniform_force_constraints) - tick_entities_local_size_base
		] };

		compile_environment.constant_definitions = 
			dispatch_command_blueprints +
			draw_arrays_command_blueprints +

			"const float meter_in_length_units = " + std::to_string(game_state::units::meter_in_length_units) + ";\n"
			"const float length_unit_in_meters = " + std::to_string(game_state::units::length_unit_in_meters) + ";\n"
			"const float second_in_time_units = " + std::to_string(game_state::units::second_in_time_units) + ";\n"
			"const float time_unit_in_seconds = " + std::to_string(game_state::units::time_unit_in_seconds) + ";\n"
			"const float meters_per_second_in_length_units_per_time_unit = " + std::to_string(game_state::units::meters_per_second_in_length_units_per_time_unit) + ";\n"
			"const float meters_per_second_squared_in_length_units_per_time_unit_squared = " + std::to_string(game_state::units::meters_per_second_squared_in_length_units_per_time_unit_squared) + ";\n"

			"const uint entity_type_count = " + std::to_string(game_state::entity_type_indices::count) + ";\n"

			"const uint dispatch_program_count = " + std::to_string(::game_state::shader_indices::tick::process_entities::count) + ";\n"
			"const uint draw_arrays_program_count = " + std::to_string(draw_arrays_program_count) + ";\n"

			"const uint point_mass_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::Indices::point_mass)) + ";\n"
			"const uint rigid_body_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::Indices::rigid_body)) + ";\n"
			"const uint point_mass_distance_constraint_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::Indices::point_mass_distance_constraint)) + ";\n"
			"const uint point_mass_uniform_force_constraint_type_index = " + std::to_string(static_cast<GLuint>(::game_state::entity_type_indices::Indices::point_mass_uniform_force_constraint)) + ";\n"
	
			"const uint uvec4_data_binding = " + std::to_string(::game_state::bindings::shader_storage::uvec4_data) + ";\n"
			"const uint uint_data_binding = " + ::std::to_string(::game_state::bindings::shader_storage::uint_data) + ";\n"
			"const uint float_data_binding = " + ::std::to_string(::game_state::bindings::shader_storage::float_data) + ";\n"
			
			"const uint update_tick_counts_local_size = " + ::std::to_string(update_tick_counts_local_size) + ";\n"
			"const uint process_point_masses_local_size = " + ::std::to_string(process_point_masses_local_size) + ";\n"
			"const uint process_rigid_bodies_local_size = " + ::std::to_string(process_rigid_bodies_local_size) + ";\n"
			"const uint process_point_mass_distance_constraints_local_size = " + ::std::to_string(process_point_mass_distance_constraints_local_size) + ";\n"
			"const uint process_point_mass_uniform_force_constraints_local_size = " + ::std::to_string(process_point_mass_uniform_force_constraints_local_size) + ";\n"
			"const uint update_draw_counts_local_size = " + ::std::to_string(update_draw_counts_local_size) + ";\n"

			"const uint fixed_data_buffer_data_size = " + ::std::to_string(environment.state.layouts.fixed_data.block_state.buffer_data_size) + ";\n"
			"const uint uvec4_data_offset = " + std::to_string(environment.state.layouts.uvec4_data.state.offset) + ";\n"
			"const uint uvec4_data_array_stride = " + std::to_string(environment.state.layouts.uvec4_data.state.array_stride) + ";\n"
			"const uint uint_data_offset = " + std::to_string(environment.state.layouts.uint_data.state.offset) + ";\n"
			"const uint uint_data_array_stride = " + std::to_string(environment.state.layouts.uint_data.state.array_stride) + ";\n"
			"const uint float_data_offset = " + std::to_string(environment.state.layouts.float_data.state.offset) + ";\n"
			"const uint float_data_array_stride = " + std::to_string(environment.state.layouts.float_data.state.array_stride) + ";\n"
		;

		compile_environment.fixed_data_source = ::util::shader::file_to_string("blocks/Fixed_Data");
		compile_environment.uvec4_data_source = ::util::shader::file_to_string("blocks/shader_storage/uvec4_Data");
		compile_environment.uint_data_source = ::util::shader::file_to_string("blocks/shader_storage/uint_Data");
		compile_environment.float_data_source = ::util::shader::file_to_string("blocks/shader_storage/float_Data");
		
		compile_environment.combined_data_source =
			compile_environment.fixed_data_source +
			compile_environment.uvec4_data_source +
			compile_environment.uint_data_source +
			compile_environment.float_data_source;

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
