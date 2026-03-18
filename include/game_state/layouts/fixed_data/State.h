#pragma once
#include "glad_glfw.h"

namespace game_state::layouts::fixed_data
{
	struct temp
	{
		GLint offset;
	};

	struct camera_position
	{
		GLint offset;
	};

	struct camera_offset_to_clip_coordinates
	{
		GLint offset;
		GLint matrix_stride;
	};

	struct point_mass_position_velocity_base
	{
		GLint offset;
	};
	
	struct point_mass_inverse_mass_base
	{
		GLint offset;
	};
	
	struct point_mass_constraint_count_base
	{
		GLint offset;
	};

	struct point_mass_read_acceleration_turn_flags_base
	{
		GLint offset;
	};

	struct point_mass_read_deltas_base
	{
		GLint offset;
	};

	struct point_mass_write_acceleration_turn_flags_base
	{
		GLint offset;
	};

	struct point_mass_write_deltas_base
	{
		GLint offset;
	};

	struct point_mass_capacity
	{
		GLint offset;
	};

	struct rigid_body_circle_contact_constraint_spawner_killed_ring_base
	{
		GLint offset;
	};

	struct rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base
	{
		GLint offset;
	};

	struct point_mass_distance_constraint_flags_target_distance_base
	{
		GLint offset;
	};

	struct point_mass_distance_constraint_indices_turns_base
	{
		GLint offset;
	};

	struct point_mass_distance_constraint_target_distance_base
	{
		GLint offset;
	};

	struct point_mass_distance_constraint_applied_accelerations_base
	{
		GLint offset;
	};

	struct point_mass_distance_constraint_delta_velocities_base
	{
		GLint offset;
	};

	struct point_mass_distance_constraint_delta_positions_base
	{
		GLint offset;
	};

	struct point_mass_distance_constraint_capacity
	{
		GLint offset;
	};

	struct point_mass_uniform_force_constraint_indices_turns_base
	{
		GLint offset;
	};

	struct point_mass_uniform_force_constraint_target_and_applied_accelerations_base
	{
		GLint offset;
	};

	struct point_mass_uniform_force_constraint_deltas_base
	{
		GLint offset;
	};

	struct point_mass_uniform_force_constraint_capacity
	{
		GLint offset;
	};

	struct rigid_body_circle_contact_constraint_killed_ring_base
	{
		GLint offset;
	};

	struct rigid_body_circle_contact_constraint_source_kill_items_ring_base
	{
		GLint offset;
	};

	struct capacities
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct old_old_kill_counts
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct old_write_counts
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct old_death_index_bases
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct old_killed_bases
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct old_kill_counts
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct old_kill_bases
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct old_kill_ends
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct read_counts
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct write_counts
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct death_index_bases
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct killed_bases
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct kill_counts
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct kill_bases
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct kill_ends
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct dispatch_command_flags
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct dispatch_commands_work_group_count_x
	{
		GLint offset;
		GLint top_level_array_stride;
		GLint top_level_array_size;
	};

	struct dispatch_commands_work_group_count_y
	{
		GLint offset;
	};
	
	struct dispatch_commands_work_group_count_z
	{
		GLint offset;
	};

	struct draw_arrays_command_flags
	{
		GLint offset;
		GLint array_stride;
		GLint array_size;
	};

	struct draw_arrays_commands_count
	{
		GLint offset;
		GLint top_level_array_stride;
		GLint top_level_array_size;
	};

	struct draw_arrays_commands_instance_count
	{
		GLint offset;
	};

	struct draw_arrays_commands_first
	{
		GLint offset;
	};

	struct draw_arrays_commands_base_instance
	{
		GLint offset;
	};

	struct Block
	{
		GLint buffer_data_size;
	};

	struct State
	{
		union
		{
			temp temp_state;
			GLint temp_properties[sizeof(temp) / sizeof(GLint)];
		};
		union
		{
			camera_position camera_position_state;
			GLint camera_position_properties[sizeof(camera_position) / sizeof(GLint)];
		};
		union
		{
			camera_offset_to_clip_coordinates camera_offset_to_clip_coordinates_state;
			GLint camera_offset_to_clip_coordinates_properties[sizeof(camera_offset_to_clip_coordinates) / sizeof(GLint)];
		};
		union
		{
			point_mass_position_velocity_base point_mass_position_velocity_base_state;
			GLint point_mass_position_velocity_base_properties[sizeof(point_mass_position_velocity_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_inverse_mass_base point_mass_inverse_mass_base_state;
			GLint point_mass_inverse_mass_base_properties[sizeof(point_mass_inverse_mass_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_constraint_count_base point_mass_constraint_count_base_state;
			GLint point_mass_constraint_count_base_properties[sizeof(point_mass_constraint_count_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_read_acceleration_turn_flags_base point_mass_read_acceleration_turn_flags_base_state;
			GLint point_mass_read_acceleration_turn_flags_base_properties[sizeof(point_mass_read_acceleration_turn_flags_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_read_deltas_base point_mass_read_deltas_base_state;
			GLint point_mass_read_deltas_base_properties[sizeof(point_mass_read_deltas_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_write_acceleration_turn_flags_base point_mass_write_acceleration_turn_flags_base_state;
			GLint point_mass_write_acceleration_turn_flags_base_properties[sizeof(point_mass_write_acceleration_turn_flags_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_write_deltas_base point_mass_write_deltas_base_state;
			GLint point_mass_write_deltas_base_properties[sizeof(point_mass_write_deltas_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_capacity point_mass_capacity_state;
			GLint point_mass_capacity_properties[sizeof(point_mass_capacity) / sizeof(GLint)];
		};
		
		union
		{
			rigid_body_circle_contact_constraint_spawner_killed_ring_base rigid_body_circle_contact_constraint_spawner_killed_ring_base_state;
			GLint rigid_body_circle_contact_constraint_spawner_killed_ring_base_properties[sizeof(rigid_body_circle_contact_constraint_spawner_killed_ring_base) / sizeof(GLint)];
		};
		union
		{
			rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base_state;
			GLint rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base_properties[sizeof(rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base) / sizeof(GLint)];
		};
		
		union
		{
			point_mass_distance_constraint_flags_target_distance_base point_mass_distance_constraint_flags_target_distance_base_state;
			GLint point_mass_distance_constraint_flags_target_distance_base_properties[sizeof(point_mass_distance_constraint_flags_target_distance_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_distance_constraint_indices_turns_base point_mass_distance_constraint_indices_turns_base_state;
			GLint point_mass_distance_constraint_indices_turns_base_properties[sizeof(point_mass_distance_constraint_indices_turns_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_distance_constraint_target_distance_base point_mass_distance_constraint_target_distance_base_state;
			GLint point_mass_distance_constraint_target_distance_base_properties[sizeof(point_mass_distance_constraint_target_distance_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_distance_constraint_applied_accelerations_base point_mass_distance_constraint_applied_accelerations_base_state;
			GLint point_mass_distance_constraint_applied_accelerations_base_properties[sizeof(point_mass_distance_constraint_applied_accelerations_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_distance_constraint_delta_velocities_base point_mass_distance_constraint_delta_velocities_base_state;
			GLint point_mass_distance_constraint_delta_velocities_base_properties[sizeof(point_mass_distance_constraint_delta_velocities_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_distance_constraint_delta_positions_base point_mass_distance_constraint_delta_positions_base_state;
			GLint point_mass_distance_constraint_delta_positions_base_properties[sizeof(point_mass_distance_constraint_delta_positions_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_distance_constraint_capacity point_mass_distance_constraint_capacity_state;
			GLint point_mass_distance_constraint_capacity_properties[sizeof(point_mass_distance_constraint_capacity) / sizeof(GLint)];
		};
		union
		{
			point_mass_uniform_force_constraint_indices_turns_base point_mass_uniform_force_constraint_indices_turns_base_state;
			GLint point_mass_uniform_force_constraint_indices_turns_base_properties[sizeof(point_mass_uniform_force_constraint_indices_turns_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_uniform_force_constraint_target_and_applied_accelerations_base point_mass_uniform_force_constraint_target_and_applied_accelerations_base_state;
			GLint point_mass_uniform_force_constraint_target_and_applied_accelerations_base_properties[sizeof(point_mass_uniform_force_constraint_target_and_applied_accelerations_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_uniform_force_constraint_deltas_base point_mass_uniform_force_constraint_deltas_base_state;
			GLint point_mass_uniform_force_constraint_deltas_base_properties[sizeof(point_mass_uniform_force_constraint_deltas_base) / sizeof(GLint)];
		};
		union
		{
			point_mass_uniform_force_constraint_capacity point_mass_uniform_force_constraint_capacity_state;
			GLint point_mass_uniform_force_constraint_capacity_properties[sizeof(point_mass_uniform_force_constraint_capacity) / sizeof(GLint)];
		};

		union
		{
			rigid_body_circle_contact_constraint_killed_ring_base rigid_body_circle_contact_constraint_killed_ring_base_state;
			GLint rigid_body_circle_contact_constraint_killed_ring_base_properties[sizeof(rigid_body_circle_contact_constraint_killed_ring_base) / sizeof(GLint)];
		};
		union
		{
			rigid_body_circle_contact_constraint_source_kill_items_ring_base rigid_body_circle_contact_constraint_source_kill_items_ring_base_state;
			GLint rigid_body_circle_contact_constraint_source_kill_items_ring_base_properties[sizeof(rigid_body_circle_contact_constraint_source_kill_items_ring_base) / sizeof(GLint)];
		};
		union
		{
			capacities capacities_state;
			GLint capacities_properties[sizeof(capacities) / sizeof(GLint)];
		};
		
		union
		{
			old_old_kill_counts old_old_kill_counts_state;
			GLint old_old_kill_counts_properties[sizeof(old_old_kill_counts) / sizeof(GLint)];
		};

		union
		{
			old_write_counts old_write_counts_state;
			GLint old_write_counts_properties[sizeof(old_write_counts) / sizeof(GLint)];
		};
		union
		{
			old_death_index_bases old_death_index_bases_state;
			GLint old_death_index_bases_properties[sizeof(old_death_index_bases) / sizeof(GLint)];
		};
		union
		{
			old_killed_bases old_killed_bases_state;
			GLint old_killed_bases_properties[sizeof(old_killed_bases) / sizeof(GLint)];
		};
		union
		{
			old_kill_counts old_kill_counts_state;
			GLint old_kill_counts_properties[sizeof(old_kill_counts) / sizeof(GLint)];
		};
		union
		{
			old_kill_bases old_kill_bases_state;
			GLint old_kill_bases_properties[sizeof(old_kill_bases) / sizeof(GLint)];
		};
		union
		{
			old_kill_ends old_kill_ends_state;
			GLint old_kill_ends_properties[sizeof(old_kill_ends) / sizeof(GLint)];
		};
		union
		{
			read_counts read_counts_state;
			GLint read_counts_properties[sizeof(read_counts) / sizeof(GLint)];
		};
		union
		{
			write_counts write_counts_state;
			GLint write_counts_properties[sizeof(write_counts) / sizeof(GLint)];
		};
		union
		{
			death_index_bases death_index_bases_state;
			GLint death_index_bases_properties[sizeof(death_index_bases) / sizeof(GLint)];
		};
		union
		{
			killed_bases killed_bases_state;
			GLint killed_bases_properties[sizeof(killed_bases) / sizeof(GLint)];
		};
		union
		{
			kill_counts kill_counts_state;
			GLint kill_counts_properties[sizeof(kill_counts) / sizeof(GLint)];
		};
		union
		{
			dispatch_command_flags dispatch_command_flags_state;
			GLint dispatch_command_flags_properties[sizeof(dispatch_command_flags) / sizeof(GLint)];
		};
		union
		{
			dispatch_commands_work_group_count_x dispatch_commands_work_group_count_x_state;
			GLint dispatch_commands_work_group_count_x_properties[sizeof(dispatch_commands_work_group_count_x) / sizeof(GLint)];
		};
		union
		{
			dispatch_commands_work_group_count_y dispatch_commands_work_group_count_y_state;
			GLint dispatch_commands_work_group_count_y_properties[sizeof(dispatch_commands_work_group_count_y) / sizeof(GLint)];
		};
		union
		{
			dispatch_commands_work_group_count_z dispatch_commands_work_group_count_z_state;
			GLint dispatch_commands_work_group_count_z_properties[sizeof(dispatch_commands_work_group_count_z) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_command_flags draw_arrays_command_flags_state;
			GLint draw_arrays_command_flags_properties[sizeof(draw_arrays_command_flags) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_commands_count draw_arrays_commands_count_state;
			GLint draw_arrays_commands_count_properties[sizeof(draw_arrays_commands_count) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_commands_instance_count draw_arrays_commands_instance_count_state;
			GLint draw_arrays_commands_instance_count_properties[sizeof(draw_arrays_commands_instance_count) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_commands_first draw_arrays_commands_first_state;
			GLint draw_arrays_commands_first_properties[sizeof(draw_arrays_commands_first) / sizeof(GLint)];
		};
		union
		{
			draw_arrays_commands_base_instance draw_arrays_commands_base_instance_state;
			GLint draw_arrays_commands_base_instance_properties[sizeof(draw_arrays_commands_base_instance) / sizeof(GLint)];
		};
		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
