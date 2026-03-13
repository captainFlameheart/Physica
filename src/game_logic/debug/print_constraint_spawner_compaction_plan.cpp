#include "game_logic/debug/include.h"

namespace game_logic::debug
{
	void print_constraint_spawner_compaction_plan(game_environment::Environment& environment)
	{
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

		GLubyte* fixed_data = new GLubyte[environment.state.layouts.fixed_data.block_state.buffer_data_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			0u, environment.state.layouts.fixed_data.block_state.buffer_data_size, fixed_data
		);

		constexpr GLuint rigid_body_circle_contact_constraint_spawner_entity_type
		{
			static_cast<GLuint>(::game_state::entity_type_indices::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawner)
		};

		GLuint rigid_body_circle_contact_constraint_spawner_killed_ring_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_killed_ring_base,
			fixed_data + environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_spawner_killed_ring_base_state.offset,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base,
			fixed_data + environment.state.layouts.fixed_data.rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base_state.offset,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_spawner_old_write_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_old_write_count,
			fixed_data +
			environment.state.layouts.fixed_data.old_write_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_entity_type * environment.state.layouts.fixed_data.old_write_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_old_kill_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_old_kill_base,
			fixed_data +
			environment.state.layouts.fixed_data.old_kill_bases_state.offset +
			rigid_body_circle_contact_constraint_spawner_entity_type * environment.state.layouts.fixed_data.old_kill_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_old_kill_end;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_old_kill_end,
			fixed_data +
			environment.state.layouts.fixed_data.old_kill_ends_state.offset +
			rigid_body_circle_contact_constraint_spawner_entity_type * environment.state.layouts.fixed_data.old_kill_ends_state.array_stride,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_spawner_read_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_read_count,
			fixed_data +
			environment.state.layouts.fixed_data.read_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_entity_type * environment.state.layouts.fixed_data.read_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_write_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_write_count,
			fixed_data +
			environment.state.layouts.fixed_data.write_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_entity_type * environment.state.layouts.fixed_data.write_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_kill_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_kill_base,
			fixed_data +
			environment.state.layouts.fixed_data.kill_bases_state.offset +
			rigid_body_circle_contact_constraint_spawner_entity_type * environment.state.layouts.fixed_data.kill_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_kill_end;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_kill_end,
			fixed_data +
			environment.state.layouts.fixed_data.kill_ends_state.offset +
			rigid_body_circle_contact_constraint_spawner_entity_type * environment.state.layouts.fixed_data.kill_ends_state.array_stride,
			sizeof(GLuint)
		);

		if (rigid_body_circle_contact_constraint_spawner_old_kill_end - rigid_body_circle_contact_constraint_spawner_old_kill_base != 0u)
		{
			std::cout << "rigid_body_circle_contact_constraint_spawner_killed_ring_base: " << rigid_body_circle_contact_constraint_spawner_killed_ring_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base: " << rigid_body_circle_contact_constraint_spawner_source_kill_items_ring_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_old_write_count: " << rigid_body_circle_contact_constraint_spawner_old_write_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_old_kill_base: " << rigid_body_circle_contact_constraint_spawner_old_kill_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_old_kill_end: " << rigid_body_circle_contact_constraint_spawner_old_kill_end << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_read_count: " << rigid_body_circle_contact_constraint_spawner_read_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_write_count: " << rigid_body_circle_contact_constraint_spawner_write_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_kill_base: " << rigid_body_circle_contact_constraint_spawner_kill_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_kill_end: " << rigid_body_circle_contact_constraint_spawner_kill_end << '\n';

			std::cout << std::endl;
		}

		delete[] fixed_data;
	}
}
