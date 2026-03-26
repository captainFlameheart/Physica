#include "game_logic/debug/include.h"

namespace game_logic::debug
{
	void print_fixed_data(game_environment::Environment& environment)
	{
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

		GLubyte* fixed_data = new GLubyte[environment.state.layouts.fixed_data.block_state.buffer_data_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			0u, environment.state.layouts.fixed_data.block_state.buffer_data_size, fixed_data
		);

		GLubyte* commands = new GLubyte[environment.state.layouts.commands.block_state.buffer_data_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.command_buffer,
			0u, environment.state.layouts.commands.block_state.buffer_data_size, commands
		);

		constexpr GLuint point_mass_distance_constraint_entity_type
		{
			static_cast<GLuint>(::game_state::entity_type_indices::constraints::Indices::rigid_body_circle_contact_constraint)
		};
		constexpr GLuint rigid_body_circle_contact_constraint_entity_type
		{
			static_cast<GLuint>(::game_state::entity_type_indices::constraints::Indices::rigid_body_circle_contact_constraint)
		};

		GLuint point_mass_inverse_mass_base;
		std::memcpy
		(
			&point_mass_inverse_mass_base,
			fixed_data + environment.state.layouts.fixed_data.point_mass_inverse_mass_base_state.offset,
			sizeof(GLuint)
		);

		GLuint point_mass_distance_constraint_flags_target_distance_base;
		std::memcpy
		(
			&point_mass_distance_constraint_flags_target_distance_base,
			fixed_data + environment.state.layouts.fixed_data.point_mass_distance_constraint_flags_target_distance_base_state.offset,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_capacity;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_capacity,
			fixed_data +
			environment.state.layouts.fixed_data.capacities_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.capacities_state.array_stride,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_old_write_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_old_write_count,
			fixed_data +
			environment.state.layouts.fixed_data.old_write_counts_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.old_write_counts_state.array_stride,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_read_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_read_count,
			fixed_data +
			environment.state.layouts.fixed_data.read_counts_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.read_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_write_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_write_count,
			fixed_data +
			environment.state.layouts.fixed_data.write_counts_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.write_counts_state.array_stride,
			sizeof(GLuint)
		);

		GLuint point_mass_distance_constraint_read_count;
		std::memcpy
		(
			&point_mass_distance_constraint_read_count,
			fixed_data +
			environment.state.layouts.fixed_data.read_counts_state.offset +
			point_mass_distance_constraint_entity_type * environment.state.layouts.fixed_data.read_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint point_mass_distance_constraint_write_count;
		std::memcpy
		(
			&point_mass_distance_constraint_write_count,
			fixed_data +
			environment.state.layouts.fixed_data.write_counts_state.offset +
			point_mass_distance_constraint_entity_type * environment.state.layouts.fixed_data.write_counts_state.array_stride,
			sizeof(GLuint)
		);

		std::cout << "Fixed Data:\n";

		std::cout << "point_mass_inverse_mass_base: " << point_mass_inverse_mass_base << '\n';
		
		std::cout << '\n';

		GLuint rigid_body_write_velocity_flags_base;
		std::memcpy
		(
			&rigid_body_write_velocity_flags_base,
			fixed_data + environment.state.layouts.fixed_data.rigid_body_write_velocity_flags_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "rigid_body_write_velocity_flags_base: " << rigid_body_write_velocity_flags_base << '\n';

		std::cout << '\n';
		
		GLuint rigid_body_circle_body_position_radius_base;
		std::memcpy
		(
			&rigid_body_circle_body_position_radius_base,
			fixed_data + environment.state.layouts.fixed_data.rigid_body_circle_body_position_radius_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "rigid_body_circle_body_position_radius_base: " << rigid_body_circle_body_position_radius_base << '\n';

		std::cout << '\n';

		std::cout << "leaf_bounding_box_parent_bases:\n";
		for (GLuint leaf_bounding_box_type{ 0u }; leaf_bounding_box_type < ::game_state::leaf_bounding_box_types::count; ++leaf_bounding_box_type)
		{
			GLuint leaf_bounding_box_parent_base;
			std::memcpy
			(
				&leaf_bounding_box_parent_base,
				fixed_data +
				environment.state.layouts.fixed_data.leaf_bounding_box_parent_bases_state.offset +
				leaf_bounding_box_type * environment.state.layouts.fixed_data.leaf_bounding_box_parent_bases_state.array_stride,
				sizeof(GLuint)
			);
			std::cout << "	" << leaf_bounding_box_parent_base << '\n';
		}

		GLuint inner_bounding_box_child_pair_base;
		std::memcpy
		(
			&inner_bounding_box_child_pair_base,
			fixed_data + environment.state.layouts.fixed_data.inner_bounding_box_child_pair_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "inner_bounding_box_child_pair_base: " << inner_bounding_box_child_pair_base << '\n';

		std::cout << "read_leaf_bounding_box_bases:\n";
		for (GLuint leaf_bounding_box_type{ 0u }; leaf_bounding_box_type < ::game_state::leaf_bounding_box_types::count; ++leaf_bounding_box_type)
		{
			GLuint read_leaf_bounding_box_base;
			std::memcpy
			(
				&read_leaf_bounding_box_base,
				fixed_data +
				environment.state.layouts.fixed_data.read_leaf_bounding_box_bases_state.offset +
				leaf_bounding_box_type * environment.state.layouts.fixed_data.read_leaf_bounding_box_bases_state.array_stride,
				sizeof(GLuint)
			);
			std::cout << "	" << read_leaf_bounding_box_base << '\n';
		}
		
		std::cout << "write_leaf_bounding_box_bases:\n";
		for (GLuint leaf_bounding_box_type{ 0u }; leaf_bounding_box_type < ::game_state::leaf_bounding_box_types::count; ++leaf_bounding_box_type)
		{
			GLuint write_leaf_bounding_box_base;
			std::memcpy
			(
				&write_leaf_bounding_box_base,
				fixed_data +
				environment.state.layouts.fixed_data.write_leaf_bounding_box_bases_state.offset +
				leaf_bounding_box_type * environment.state.layouts.fixed_data.write_leaf_bounding_box_bases_state.array_stride,
				sizeof(GLuint)
			);
			std::cout << "	" << write_leaf_bounding_box_base << '\n';
		}

		GLuint read_inner_bounding_box_base;
		std::memcpy
		(
			&read_inner_bounding_box_base,
			fixed_data + environment.state.layouts.fixed_data.read_inner_bounding_box_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "read_inner_bounding_box_base: " << read_inner_bounding_box_base << '\n';

		GLuint write_inner_bounding_box_base;
		std::memcpy
		(
			&write_inner_bounding_box_base,
			fixed_data + environment.state.layouts.fixed_data.write_inner_bounding_box_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "write_inner_bounding_box_base: " << write_inner_bounding_box_base << '\n';

		std::cout << '\n';
		
		GLuint rigid_body_circle_contact_constraint_spawner_indices_flags_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_indices_flags_base,
			fixed_data + environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_spawner_indices_flags_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "rigid_body_circle_contact_constraint_spawner_indices_flags_base: " << rigid_body_circle_contact_constraint_spawner_indices_flags_base << '\n';

		std::cout << '\n';

		std::cout << "point_mass_distance_constraint_flags_target_distance_base: " << point_mass_distance_constraint_flags_target_distance_base << '\n';
		
		GLuint point_mass_distance_constraint_indices_turns_base;
		std::memcpy
		(
			&point_mass_distance_constraint_indices_turns_base,
			fixed_data + environment.state.layouts.fixed_data.point_mass_distance_constraint_indices_turns_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "point_mass_distance_constraint_indices_turns_base: " << point_mass_distance_constraint_indices_turns_base << '\n';

		std::cout << '\n';
		
		std::cout << "point_mass_distance_constraint_read_count: " << point_mass_distance_constraint_read_count << '\n';
		std::cout << "point_mass_distance_constraint_write_count: " << point_mass_distance_constraint_write_count << '\n';
		
		std::cout << '\n';
		
		std::cout << "rigid_body_circle_contact_constraint_capacity: " << rigid_body_circle_contact_constraint_capacity << '\n';
		std::cout << "rigid_body_circle_contact_constraint_old_write_count: " << rigid_body_circle_contact_constraint_old_write_count << '\n';
		std::cout << "rigid_body_circle_contact_constraint_read_count: " << rigid_body_circle_contact_constraint_read_count << '\n';
		std::cout << "rigid_body_circle_contact_constraint_write_count: " << rigid_body_circle_contact_constraint_write_count << '\n';
		
		std::cout << '\n';

		std::cout << "Work group counts: " << '\n';
		for (GLuint i{ 0u }; i < ::game_state::shader_indices::tick::process_entities::count; ++i)
		{
			GLuint work_group_count_x;
			std::memcpy
			(
				&work_group_count_x,
				commands +
				environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.offset +
				i * environment.state.layouts.commands.dispatch_commands_work_group_count_x_state.top_level_array_stride,
				sizeof(GLuint)
			);
			std::cout << "work_group_count_x: " << work_group_count_x << '\n';
		}

		std::cout << '\n';
	}
}
