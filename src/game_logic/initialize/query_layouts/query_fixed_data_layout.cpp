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
			program, "Fixed_Data.point_mass_position_velocity_base",
			environment.state.layouts.fixed_data.point_mass_position_velocity_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_inverse_mass_base",
			environment.state.layouts.fixed_data.point_mass_inverse_mass_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_constraint_count_base",
			environment.state.layouts.fixed_data.point_mass_constraint_count_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_read_acceleration_turn_flags_base",
			environment.state.layouts.fixed_data.point_mass_read_acceleration_turn_flags_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_read_deltas_base",
			environment.state.layouts.fixed_data.point_mass_read_deltas_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_write_acceleration_turn_flags_base",
			environment.state.layouts.fixed_data.point_mass_write_acceleration_turn_flags_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_write_deltas_base",
			environment.state.layouts.fixed_data.point_mass_write_deltas_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_capacity",
			environment.state.layouts.fixed_data.point_mass_capacity_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_distance_constraint_flags_base",
			environment.state.layouts.fixed_data.point_mass_distance_constraint_flags_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_distance_constraint_indices_turns_base",
			environment.state.layouts.fixed_data.point_mass_distance_constraint_indices_turns_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_distance_constraint_target_distance_base",
			environment.state.layouts.fixed_data.point_mass_distance_constraint_target_distance_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_distance_constraint_applied_accelerations_base",
			environment.state.layouts.fixed_data.point_mass_distance_constraint_applied_accelerations_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_distance_constraint_delta_velocities_base",
			environment.state.layouts.fixed_data.point_mass_distance_constraint_delta_velocities_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_distance_constraint_delta_positions_base",
			environment.state.layouts.fixed_data.point_mass_distance_constraint_delta_positions_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_distance_constraint_capacity",
			environment.state.layouts.fixed_data.point_mass_distance_constraint_capacity_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_uniform_force_constraint_indices_turns_base",
			environment.state.layouts.fixed_data.point_mass_uniform_force_constraint_indices_turns_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_uniform_force_constraint_target_and_applied_accelerations_base",
			environment.state.layouts.fixed_data.point_mass_uniform_force_constraint_target_and_applied_accelerations_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_uniform_force_constraint_deltas_base",
			environment.state.layouts.fixed_data.point_mass_uniform_force_constraint_deltas_base_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.point_mass_uniform_force_constraint_capacity",
			environment.state.layouts.fixed_data.point_mass_uniform_force_constraint_capacity_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.dispatch_command_flags",
			environment.state.layouts.fixed_data.dispatch_command_flags_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_top_level_array_stride_top_level_array_size
		(
			program, "Fixed_Data.dispatch_commands[0].work_group_count_x",
			environment.state.layouts.fixed_data.dispatch_commands_work_group_count_x_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.dispatch_commands[0].work_group_count_y",
			environment.state.layouts.fixed_data.dispatch_commands_work_group_count_y_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.dispatch_commands[0].work_group_count_z",
			environment.state.layouts.fixed_data.dispatch_commands_work_group_count_z_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_array_stride_array_size
		(
			program, "Fixed_Data.draw_arrays_command_flags",
			environment.state.layouts.fixed_data.draw_arrays_command_flags_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset_top_level_array_stride_top_level_array_size
		(
			program, "Fixed_Data.draw_arrays_commands[0].count",
			environment.state.layouts.fixed_data.draw_arrays_commands_count_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.draw_arrays_commands[0].instance_count",
			environment.state.layouts.fixed_data.draw_arrays_commands_instance_count_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.draw_arrays_commands[0].first",
			environment.state.layouts.fixed_data.draw_arrays_commands_first_properties
		);
		::game_logic::shader_util::query::query_shader_storage_variable_offset
		(
			program, "Fixed_Data.draw_arrays_commands[0].base_instance",
			environment.state.layouts.fixed_data.draw_arrays_commands_base_instance_properties
		);
		::game_logic::shader_util::query::query_shader_storage_buffer_data_size
		(
			program, "Fixed_Data",
			environment.state.layouts.fixed_data.block_properties
		);
	}
}
