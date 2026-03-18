#include "game_logic/debug/include.h"

namespace game_logic::debug
{
	void print_constraint_compaction_plan(game_environment::Environment& environment)
	{
		/*glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

		GLubyte* fixed_data = new GLubyte[environment.state.layouts.fixed_data.block_state.buffer_data_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			0u, environment.state.layouts.fixed_data.block_state.buffer_data_size, fixed_data
		);

		constexpr GLuint rigid_body_circle_contact_constraint_entity_type
		{
			static_cast<GLuint>(::game_state::entity_type_indices::constraints::Indices::rigid_body_circle_contact_constraint)
		};

		GLuint rigid_body_circle_contact_constraint_killed_ring_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_killed_ring_base,
			fixed_data + environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_killed_ring_base_state.offset,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_source_kill_items_ring_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_source_kill_items_ring_base,
			fixed_data + environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_source_kill_items_ring_base_state.offset,
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
		GLuint rigid_body_circle_contact_constraint_old_kill_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_old_kill_base,
			fixed_data +
			environment.state.layouts.fixed_data.old_kill_bases_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.old_kill_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_old_kill_end;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_old_kill_end,
			fixed_data +
			environment.state.layouts.fixed_data.old_kill_ends_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.old_kill_ends_state.array_stride,
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
		GLuint rigid_body_circle_contact_constraint_kill_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_kill_base,
			fixed_data +
			environment.state.layouts.fixed_data.kill_bases_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.kill_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_kill_end;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_kill_end,
			fixed_data +
			environment.state.layouts.fixed_data.kill_ends_state.offset +
			rigid_body_circle_contact_constraint_entity_type * environment.state.layouts.fixed_data.kill_ends_state.array_stride,
			sizeof(GLuint)
		);

		GLuint temp;
		std::memcpy
		(
			&temp,
			fixed_data + environment.state.layouts.fixed_data.temp_state.offset,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_kill_count{ rigid_body_circle_contact_constraint_old_kill_end - rigid_body_circle_contact_constraint_old_kill_base };
		if (rigid_body_circle_contact_constraint_kill_count != 0u)
		{
			GLuint rigid_body_circle_contact_constraint_killed_ring_size{ rigid_body_circle_contact_constraint_capacity * environment.state.layouts.uint_data.state.array_stride };
			GLubyte* rigid_body_circle_contact_constraint_killed_ring = new GLubyte[rigid_body_circle_contact_constraint_killed_ring_size];
			glGetNamedBufferSubData
			(
				environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
				environment.state.layouts.uint_data.state.offset + rigid_body_circle_contact_constraint_killed_ring_base * environment.state.layouts.uint_data.state.array_stride,
				rigid_body_circle_contact_constraint_killed_ring_size, rigid_body_circle_contact_constraint_killed_ring
			);

			GLuint rigid_body_circle_contact_constraint_source_kill_items_ring_size{ rigid_body_circle_contact_constraint_capacity * environment.state.layouts.uint_data.state.array_stride };
			GLubyte* rigid_body_circle_contact_constraint_source_kill_items_ring = new GLubyte[rigid_body_circle_contact_constraint_source_kill_items_ring_size];
			glGetNamedBufferSubData
			(
				environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
				environment.state.layouts.uint_data.state.offset + rigid_body_circle_contact_constraint_source_kill_items_ring_base * environment.state.layouts.uint_data.state.array_stride,
				rigid_body_circle_contact_constraint_source_kill_items_ring_size, rigid_body_circle_contact_constraint_source_kill_items_ring
			);

			std::cout << "temp: " << temp << '\n';
			std::cout << "rigid_body_circle_contact_constraint_killed_ring_base: " << rigid_body_circle_contact_constraint_killed_ring_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_source_kill_items_ring_base: " << rigid_body_circle_contact_constraint_source_kill_items_ring_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_capacity: " << rigid_body_circle_contact_constraint_capacity << '\n';
			std::cout << "rigid_body_circle_contact_constraint_old_write_count: " << rigid_body_circle_contact_constraint_old_write_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_old_kill_base: " << rigid_body_circle_contact_constraint_old_kill_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_old_kill_end: " << rigid_body_circle_contact_constraint_old_kill_end << '\n';
			std::cout << "rigid_body_circle_contact_constraint_read_count: " << rigid_body_circle_contact_constraint_read_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_write_count: " << rigid_body_circle_contact_constraint_write_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_kill_base: " << rigid_body_circle_contact_constraint_kill_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_kill_end: " << rigid_body_circle_contact_constraint_kill_end << '\n';
			std::cout << '\n';

			std::cout << "rigid_body_circle_contact_constraint_killed:" << '\n';
			for (GLuint kill_item{ 0u }; kill_item < rigid_body_circle_contact_constraint_kill_count; ++kill_item)
			{
				GLuint kill_item_ring_index{ rigid_body_circle_contact_constraint_old_kill_base + kill_item };
				kill_item_ring_index -= static_cast<GLuint>(kill_item_ring_index >= rigid_body_circle_contact_constraint_capacity) * rigid_body_circle_contact_constraint_capacity;

				GLuint killed;
				std::memcpy
				(
					&killed,
					rigid_body_circle_contact_constraint_killed_ring +
					kill_item_ring_index * environment.state.layouts.uint_data.state.array_stride,
					sizeof(GLuint)
				);
				GLuint source_kill_item;
				std::memcpy
				(
					&source_kill_item,
					rigid_body_circle_contact_constraint_source_kill_items_ring +
					kill_item_ring_index * environment.state.layouts.uint_data.state.array_stride,
					sizeof(GLuint)
				);

				GLuint destination_kill_item_ring_index = rigid_body_circle_contact_constraint_old_write_count - 1u - killed;
				destination_kill_item_ring_index += rigid_body_circle_contact_constraint_old_kill_base;
				destination_kill_item_ring_index -= static_cast<GLuint>(destination_kill_item_ring_index >= rigid_body_circle_contact_constraint_capacity) * rigid_body_circle_contact_constraint_capacity;
				std::cout << rigid_body_circle_contact_constraint_old_write_count - rigid_body_circle_contact_constraint_kill_count << std::endl;
				std::cout << destination_kill_item_ring_index << std::endl;

				std::cout
					<< "kill_item: " << kill_item
					<< ", kill_item_ring_index: " << kill_item_ring_index
					<< ", killed: " << killed
					<< ", source_kill_item: " << source_kill_item
					<< '\n';
			}

			std::cout << std::endl;

			delete[] rigid_body_circle_contact_constraint_killed_ring;
			delete[] rigid_body_circle_contact_constraint_source_kill_items_ring;
		}

		delete[] fixed_data;*/
	}
}
