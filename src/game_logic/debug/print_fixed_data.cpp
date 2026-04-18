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
		constexpr GLuint rigid_body_circle_type
		{
			static_cast<GLuint>(::game_state::entity_type_indices::body_attachments::Indices::rigid_body_circle)
		};
		constexpr GLuint rigid_body_circle_bounding_box_type
		{
			static_cast<GLuint>(::game_state::leaf_bounding_box_types::Indices::rigid_body_circle)
		};
		constexpr GLuint inner_bounding_box_type
		{
			static_cast<GLuint>(::game_state::entity_type_indices::bounding_volume_hierarchy::Indices::inner_bounding_box)
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

		GLuint rigid_body_circle_capacity;
		std::memcpy
		(
			&rigid_body_circle_capacity,
			fixed_data +
			environment.state.layouts.fixed_data.capacities_state.offset +
			rigid_body_circle_type * environment.state.layouts.fixed_data.capacities_state.array_stride,
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

		std::cout << "leaf_bounding_box_parent_pad_pad_type_bases:\n";
		GLuint leaf_bounding_box_parent_pad_pad_type_bases[::game_state::leaf_bounding_box_types::count];
		for (GLuint leaf_bounding_box_type{ 0u }; leaf_bounding_box_type < ::game_state::leaf_bounding_box_types::count; ++leaf_bounding_box_type)
		{
			GLuint leaf_bounding_box_parent_base;
			std::memcpy
			(
				&leaf_bounding_box_parent_base,
				fixed_data +
				environment.state.layouts.fixed_data.leaf_bounding_box_parent_pad_pad_type_bases_state.offset +
				leaf_bounding_box_type * environment.state.layouts.fixed_data.leaf_bounding_box_parent_pad_pad_type_bases_state.array_stride,
				sizeof(GLuint)
			);
			leaf_bounding_box_parent_pad_pad_type_bases[leaf_bounding_box_type] = leaf_bounding_box_parent_base;
			std::cout << "	" << leaf_bounding_box_parent_base << '\n';
		}

		GLuint inner_bounding_box_parent_children_height_base;
		std::memcpy
		(
			&inner_bounding_box_parent_children_height_base,
			fixed_data + environment.state.layouts.fixed_data.inner_bounding_box_parent_children_height_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "inner_bounding_box_parent_children_height_base: " << inner_bounding_box_parent_children_height_base << '\n';

		std::cout << "leaf_bounding_box_bases:\n";
		GLuint leaf_bounding_box_bases[::game_state::leaf_bounding_box_types::count];
		for (GLuint leaf_bounding_box_type{ 0u }; leaf_bounding_box_type < ::game_state::leaf_bounding_box_types::count; ++leaf_bounding_box_type)
		{
			GLuint leaf_bounding_box_base;
			std::memcpy
			(
				&leaf_bounding_box_base,
				fixed_data +
				environment.state.layouts.fixed_data.leaf_bounding_box_bases_state.offset +
				leaf_bounding_box_type * environment.state.layouts.fixed_data.leaf_bounding_box_bases_state.array_stride,
				sizeof(GLuint)
			);
			leaf_bounding_box_bases[leaf_bounding_box_type] = leaf_bounding_box_base;
			std::cout << "	" << leaf_bounding_box_base << '\n';
		}
		
		GLuint inner_bounding_box_base;
		std::memcpy
		(
			&inner_bounding_box_base,
			fixed_data + environment.state.layouts.fixed_data.inner_bounding_box_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "inner_bounding_box_base: " << inner_bounding_box_base << '\n';

		std::cout << '\n';

		GLuint inner_bounding_box_height_delimiters_base;
		std::memcpy
		(
			&inner_bounding_box_height_delimiters_base,
			fixed_data + environment.state.layouts.fixed_data.inner_bounding_box_height_delimiters_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "inner_bounding_box_height_delimiters_base: " << inner_bounding_box_height_delimiters_base << '\n';

		GLuint inner_bounding_box_migration_list_base;
		std::memcpy
		(
			&inner_bounding_box_migration_list_base,
			fixed_data + environment.state.layouts.fixed_data.inner_bounding_box_migration_list_base_state.offset,
			sizeof(GLuint)
		);
		std::cout << "inner_bounding_box_migration_list_base: " << inner_bounding_box_migration_list_base << '\n';

		GLuint inner_bounding_box_migration_count;
		std::memcpy
		(
			&inner_bounding_box_migration_count,
			fixed_data + environment.state.layouts.fixed_data.inner_bounding_box_migration_count_state.offset,
			sizeof(GLuint)
		);
		std::cout << "inner_bounding_box_migration_count: " << inner_bounding_box_migration_count << '\n';

		std::cout << '\n';

		GLuint current_inner_bounding_box_height;
		std::memcpy
		(
			&current_inner_bounding_box_height,
			fixed_data + environment.state.layouts.fixed_data.current_inner_bounding_box_height_state.offset,
			sizeof(GLuint)
		);
		std::cout << "current_inner_bounding_box_height: " << current_inner_bounding_box_height << '\n';

		GLuint current_inner_bounding_box_height_start;
		std::memcpy
		(
			&current_inner_bounding_box_height_start,
			fixed_data + environment.state.layouts.fixed_data.current_inner_bounding_box_height_start_state.offset,
			sizeof(GLuint)
		);
		std::cout << "current_inner_bounding_box_height_start: " << current_inner_bounding_box_height_start << '\n';

		GLuint current_inner_bounding_box_height_count;
		std::memcpy
		(
			&current_inner_bounding_box_height_count,
			fixed_data + environment.state.layouts.fixed_data.current_inner_bounding_box_height_count_state.offset,
			sizeof(GLuint)
		);
		std::cout << "current_inner_bounding_box_height_count: " << current_inner_bounding_box_height_count << '\n';

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

		std::cout << "Entity types:\n";
		for (GLuint entity_type{ 0u }; entity_type < ::game_state::entity_type_indices::count; ++entity_type)
		{
			std::cout << ::game_state::entity_type_indices::names[entity_type] << ":\n";

			GLuint write_count;
			std::memcpy
			(
				&write_count,
				fixed_data +
				environment.state.layouts.fixed_data.write_counts_state.offset +
				entity_type * environment.state.layouts.fixed_data.write_counts_state.array_stride,
				sizeof(GLuint)
			);
			std::cout << "write_count: " << write_count << std::endl;

			std::cout << '\n';
		}

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

		GLuint rigid_body_circle_parent_pad_pad_types_size{ rigid_body_circle_capacity * 16u };
		GLubyte* rigid_body_circle_parent_pad_pad_types = new GLubyte[rigid_body_circle_parent_pad_pad_types_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			environment.state.layouts.uvec4_data.state.offset + leaf_bounding_box_parent_pad_pad_type_bases[rigid_body_circle_bounding_box_type] * environment.state.layouts.uvec4_data.state.array_stride,
			rigid_body_circle_parent_pad_pad_types_size,
			rigid_body_circle_parent_pad_pad_types
		);

		GLuint rigid_body_circle_bounding_boxes_size{ rigid_body_circle_capacity * 16u };
		GLubyte* rigid_body_circle_bounding_boxes = new GLubyte[rigid_body_circle_bounding_boxes_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			environment.state.layouts.uvec4_data.state.offset + leaf_bounding_box_bases[rigid_body_circle_bounding_box_type] * environment.state.layouts.uvec4_data.state.array_stride,
			rigid_body_circle_bounding_boxes_size,
			rigid_body_circle_bounding_boxes
		);

		GLuint rigid_body_circle_read_count;
		std::memcpy
		(
			&rigid_body_circle_read_count,
			fixed_data +
			environment.state.layouts.fixed_data.read_counts_state.offset +
			rigid_body_circle_type * environment.state.layouts.fixed_data.read_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_write_count;
		std::memcpy
		(
			&rigid_body_circle_write_count,
			fixed_data +
			environment.state.layouts.fixed_data.write_counts_state.offset +
			rigid_body_circle_type * environment.state.layouts.fixed_data.write_counts_state.array_stride,
			sizeof(GLuint)
		);
		std::cout << "Rigid body circles:\n";
		for (GLuint rigid_body_circle{ 0u }; rigid_body_circle < rigid_body_circle_write_count; ++rigid_body_circle)
		{
			GLuint rigid_body_circle_parent_pad_pad_type[4u];
			std::memcpy
			(
				&rigid_body_circle_parent_pad_pad_type,
				rigid_body_circle_parent_pad_pad_types + rigid_body_circle * environment.state.layouts.uvec4_data.state.array_stride,
				sizeof(GLuint[4u])
			);
			std::cout << "parent_pad_pad_type: ("
				<< rigid_body_circle_parent_pad_pad_type[0u] << ", "
				<< rigid_body_circle_parent_pad_pad_type[1u] << ", "
				<< rigid_body_circle_parent_pad_pad_type[2u] << ", "
				<< rigid_body_circle_parent_pad_pad_type[3u] << ")\n";

			GLuint rigid_body_circle_bounding_box[4u];
			std::memcpy
			(
				&rigid_body_circle_bounding_box,
				rigid_body_circle_bounding_boxes + rigid_body_circle * environment.state.layouts.uvec4_data.state.array_stride,
				sizeof(GLuint[4u])
			);
			std::cout << "bounding_box: ("
				<< rigid_body_circle_bounding_box[0u] << ", "
				<< rigid_body_circle_bounding_box[1u] << ", "
				<< rigid_body_circle_bounding_box[2u] << ", "
				<< rigid_body_circle_bounding_box[3u] << ")\n";
		}

		delete[] rigid_body_circle_parent_pad_pad_types;
		delete[] rigid_body_circle_bounding_boxes;

		std::cout << '\n';

		GLuint inner_bounding_box_parent_children_heights_size{ rigid_body_circle_capacity * 16u };
		GLubyte* inner_bounding_box_parent_children_heights = new GLubyte[inner_bounding_box_parent_children_heights_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			environment.state.layouts.uvec4_data.state.offset + inner_bounding_box_parent_children_height_base * environment.state.layouts.uvec4_data.state.array_stride,
			inner_bounding_box_parent_children_heights_size,
			inner_bounding_box_parent_children_heights
		);

		GLuint leaf_bounding_box_capacity = rigid_body_circle_capacity;
		GLuint inner_bounding_box_capacity = leaf_bounding_box_capacity - 1u;
		GLuint inner_bounding_boxes_size{ inner_bounding_box_capacity * 16u };
		GLubyte* inner_bounding_boxes = new GLubyte[inner_bounding_boxes_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			environment.state.layouts.uvec4_data.state.offset + inner_bounding_box_base * environment.state.layouts.uvec4_data.state.array_stride,
			inner_bounding_boxes_size,
			inner_bounding_boxes
		);

		GLuint inner_bounding_box_height_delimiters_size{ inner_bounding_box_capacity * sizeof(GLuint) };
		GLubyte* inner_bounding_box_height_delimiters = new GLubyte[inner_bounding_box_height_delimiters_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			environment.state.layouts.uint_data.state.offset + inner_bounding_box_height_delimiters_base * environment.state.layouts.uint_data.state.array_stride,
			inner_bounding_box_height_delimiters_size,
			inner_bounding_box_height_delimiters
		);

		GLuint inner_bounding_box_read_count;
		std::memcpy
		(
			&inner_bounding_box_read_count,
			fixed_data +
			environment.state.layouts.fixed_data.read_counts_state.offset +
			inner_bounding_box_type * environment.state.layouts.fixed_data.read_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint inner_bounding_box_write_count;
		std::memcpy
		(
			&inner_bounding_box_write_count,
			fixed_data +
			environment.state.layouts.fixed_data.write_counts_state.offset +
			inner_bounding_box_type * environment.state.layouts.fixed_data.write_counts_state.array_stride,
			sizeof(GLuint)
		);

		std::cout << "Inner bounding boxes:\n";
		for (GLuint inner_bounding_box{ 0u }; inner_bounding_box < inner_bounding_box_write_count; ++inner_bounding_box)
		{
			GLuint inner_bounding_box_parent_children_height[4u];
			std::memcpy
			(
				&inner_bounding_box_parent_children_height,
				inner_bounding_box_parent_children_heights + inner_bounding_box * environment.state.layouts.uvec4_data.state.array_stride,
				sizeof(GLuint[4u])
			);
			std::cout << inner_bounding_box << ": parent_children_height: ("
				<< inner_bounding_box_parent_children_height[0u] << ", "
				<< inner_bounding_box_parent_children_height[1u] << ", "
				<< inner_bounding_box_parent_children_height[2u] << ", "
				<< inner_bounding_box_parent_children_height[3u] << ")\n";

			GLuint bounding_box[4u];
			std::memcpy
			(
				&bounding_box,
				inner_bounding_boxes + inner_bounding_box * environment.state.layouts.uvec4_data.state.array_stride,
				sizeof(GLuint[4u])
			);
			/*std::cout << "bounding_box: ("
				<< bounding_box[0u] << ", "
				<< bounding_box[1u] << ", "
				<< bounding_box[2u] << ", "
				<< bounding_box[3u] << ")\n";*/
		}

		std::cout << '\n';

		std::cout << "Inner bounding box height delimiters:\n";
		for (GLuint height_level{ 0u }; height_level < inner_bounding_box_write_count; ++height_level)
		{
			GLuint delimiter{ inner_bounding_box_height_delimiters[height_level] };
			std::memcpy
			(
				&delimiter,
				inner_bounding_box_height_delimiters + height_level * environment.state.layouts.uint_data.state.array_stride,
				sizeof(GLuint)
			);
			std::cout << height_level << ": " << delimiter << '\n';
		}

		std::cout << '\n';

		std::cout << "Inner bounding box grouping:\n";
		GLuint inner_bounding_box_index{ 0u };
		for (GLuint height_level{ 0u }; height_level < inner_bounding_box_write_count; ++height_level)
		{
			GLuint delimiter{ inner_bounding_box_height_delimiters[height_level] };
			std::memcpy
			(
				&delimiter,
				inner_bounding_box_height_delimiters + height_level * environment.state.layouts.uint_data.state.array_stride,
				sizeof(GLuint)
			);
			while (inner_bounding_box_index < delimiter)
			{
				GLuint inner_bounding_box_parent_children_height[4u];
				std::memcpy
				(
					&inner_bounding_box_parent_children_height,
					inner_bounding_box_parent_children_heights + inner_bounding_box_index * environment.state.layouts.uvec4_data.state.array_stride,
					sizeof(GLuint[4u])
				);
				std::cout << inner_bounding_box_parent_children_height[3u] << ' ';

				++inner_bounding_box_index;
			}
			std::cout << "| ";
		}
		std::cout << "\n\n";

		delete[] inner_bounding_box_parent_children_heights;
		delete[] inner_bounding_boxes;
		delete[] inner_bounding_box_height_delimiters;

		GLuint temp;
		std::memcpy
		(
			&temp,
			fixed_data + environment.state.layouts.fixed_data.temp_state.offset,
			sizeof(GLuint)
		);
		std::cout << "Temp: " << temp << std::endl;

		std::cout << std::endl;

		delete[] fixed_data;
		delete[] commands;
	}
}
