#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"
#include "game_logic/shader_util/query/include.h"
#include <iostream>

namespace game_logic::initialize::query_layouts
{
	void query_fixed_data_layout(game_environment::Environment& environment, GLuint program)
	{
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.temp",
			environment.state.layouts.fixed_data.temp_properties
		);

		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.camera_position",
			environment.state.layouts.fixed_data.camera_position_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_matrix_stride
		(
			program, "Fixed_Data.camera_offset_to_clip_coordinates",
			environment.state.layouts.fixed_data.camera_offset_to_clip_coordinates_properties
		);
		
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.rigid_body_circle_contact_constraint_spawner_killed_ring_base",
			environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_spawner_killed_ring_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base",
			environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base_properties
		);

		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.rigid_body_circle_contact_constraint_killed_ring_base",
			environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_killed_ring_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.rigid_body_circle_contact_constraint_source_kill_items_ring_base",
			environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_source_kill_items_ring_base_properties
		);
		
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.capacities",
			environment.state.layouts.fixed_data.capacities_properties
		);

		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.old_write_counts",
			environment.state.layouts.fixed_data.old_write_counts_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.old_kill_bases",
			environment.state.layouts.fixed_data.old_kill_bases_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.old_kill_ends",
			environment.state.layouts.fixed_data.old_kill_ends_properties
		);

		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.read_counts",
			environment.state.layouts.fixed_data.read_counts_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.write_counts",
			environment.state.layouts.fixed_data.write_counts_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.kill_bases",
			environment.state.layouts.fixed_data.kill_bases_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.kill_ends",
			environment.state.layouts.fixed_data.kill_ends_properties
		);

		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.dispatch_command_flags",
			environment.state.layouts.fixed_data.dispatch_command_flags_properties
		);
		
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.draw_arrays_command_flags",
			environment.state.layouts.fixed_data.draw_arrays_command_flags_properties
		);
		
		::game_logic::shader_util::query::query_shader_storage_buffer_data_size
		(
			program, "Fixed_Data",
			environment.state.layouts.fixed_data.block_properties
		);
	}
}
