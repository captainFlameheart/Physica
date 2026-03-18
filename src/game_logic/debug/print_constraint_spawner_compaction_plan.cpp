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

		constexpr GLuint rigid_body_circle_contact_constraint_spawner_type
		{
			static_cast<GLuint>(::game_state::entity_type_indices::constraint_spawners::Indices::rigid_body_circle_contact_constraint_spawner)
		};

		GLuint rigid_body_circle_contact_constraint_spawner_capacity;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_capacity,
			fixed_data + environment.state.layouts.fixed_data.capacities_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.capacities_state.array_stride,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_spawner_old_old_kill_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_old_old_kill_count,
			fixed_data + environment.state.layouts.fixed_data.old_old_kill_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.old_old_kill_counts_state.array_stride,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_spawner_old_write_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_old_write_count,
			fixed_data + environment.state.layouts.fixed_data.old_write_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.old_write_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint old_killed_rigid_body_circle_contact_constraint_spawners_base;
		std::memcpy
		(
			&old_killed_rigid_body_circle_contact_constraint_spawners_base,
			fixed_data + environment.state.layouts.fixed_data.old_killed_bases_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.old_killed_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_old_death_index_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_old_death_index_base,
			fixed_data + environment.state.layouts.fixed_data.old_death_index_bases_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.old_death_index_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_old_kill_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_old_kill_count,
			fixed_data + environment.state.layouts.fixed_data.old_kill_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.old_kill_counts_state.array_stride,
			sizeof(GLuint)
		);

		GLuint rigid_body_circle_contact_constraint_spawner_read_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_read_count,
			fixed_data + environment.state.layouts.fixed_data.read_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.read_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_write_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_write_count,
			fixed_data + environment.state.layouts.fixed_data.write_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.write_counts_state.array_stride,
			sizeof(GLuint)
		);
		GLuint killed_rigid_body_circle_contact_constraint_spawners_base;
		std::memcpy
		(
			&killed_rigid_body_circle_contact_constraint_spawners_base,
			fixed_data + environment.state.layouts.fixed_data.killed_bases_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.killed_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_death_index_base;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_death_index_base,
			fixed_data + environment.state.layouts.fixed_data.death_index_bases_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.death_index_bases_state.array_stride,
			sizeof(GLuint)
		);
		GLuint rigid_body_circle_contact_constraint_spawner_kill_count;
		std::memcpy
		(
			&rigid_body_circle_contact_constraint_spawner_kill_count,
			fixed_data + environment.state.layouts.fixed_data.kill_counts_state.offset +
			rigid_body_circle_contact_constraint_spawner_type * environment.state.layouts.fixed_data.kill_counts_state.array_stride,
			sizeof(GLuint)
		);

		GLuint temp;
		std::memcpy
		(
			&temp,
			fixed_data + environment.state.layouts.fixed_data.temp_state.offset,
			sizeof(GLuint)
		);

		std::cout << "rigid_body_circle_contact_constraint_spawner_old_write_count: " << rigid_body_circle_contact_constraint_spawner_old_write_count << '\n';
		std::cout << "rigid_body_circle_contact_constraint_spawner_write_count: " << rigid_body_circle_contact_constraint_spawner_write_count << '\n';
		std::cout << '\n';

		if (rigid_body_circle_contact_constraint_spawner_old_kill_count != 0u)
		{
			GLuint old_killed_rigid_body_circle_contact_constraint_spawners_size{ rigid_body_circle_contact_constraint_spawner_capacity * environment.state.layouts.uint_data.state.array_stride };
			GLubyte* old_killed_rigid_body_circle_contact_constraint_spawners{ new GLubyte[old_killed_rigid_body_circle_contact_constraint_spawners_size] };
			glGetNamedBufferSubData
			(
				environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
				environment.state.layouts.uint_data.state.offset + old_killed_rigid_body_circle_contact_constraint_spawners_base * environment.state.layouts.uint_data.state.array_stride,
				old_killed_rigid_body_circle_contact_constraint_spawners_size, old_killed_rigid_body_circle_contact_constraint_spawners
			);

			GLuint rigid_body_circle_contact_constraint_spawner_old_death_indices_size{ rigid_body_circle_contact_constraint_spawner_capacity * environment.state.layouts.uint_data.state.array_stride };
			GLubyte* rigid_body_circle_contact_constraint_spawner_old_death_indices{ new GLubyte[rigid_body_circle_contact_constraint_spawner_old_death_indices_size] };
			glGetNamedBufferSubData
			(
				environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
				environment.state.layouts.uint_data.state.offset + rigid_body_circle_contact_constraint_spawner_old_death_index_base * environment.state.layouts.uint_data.state.array_stride,
				rigid_body_circle_contact_constraint_spawner_old_death_indices_size, rigid_body_circle_contact_constraint_spawner_old_death_indices
			);

			std::cout << "temp: " << temp << '\n';
			std::cout << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_capacity: " << rigid_body_circle_contact_constraint_spawner_capacity << '\n';
			std::cout << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_old_old_kill_count: " << rigid_body_circle_contact_constraint_spawner_old_old_kill_count << '\n';
			std::cout << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_old_write_count: " << rigid_body_circle_contact_constraint_spawner_old_write_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_old_death_index_base: " << rigid_body_circle_contact_constraint_spawner_old_death_index_base << '\n';
			std::cout << "old_killed_rigid_body_circle_contact_constraint_spawners_base: " << old_killed_rigid_body_circle_contact_constraint_spawners_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_old_kill_count: " << rigid_body_circle_contact_constraint_spawner_old_kill_count << '\n';
			std::cout << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_read_count: " << rigid_body_circle_contact_constraint_spawner_read_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_write_count: " << rigid_body_circle_contact_constraint_spawner_write_count << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_death_index_base: " << rigid_body_circle_contact_constraint_spawner_death_index_base << '\n';
			std::cout << "killed_rigid_body_circle_contact_constraint_spawners_base: " << killed_rigid_body_circle_contact_constraint_spawners_base << '\n';
			std::cout << "rigid_body_circle_contact_constraint_spawner_kill_count: " << rigid_body_circle_contact_constraint_spawner_kill_count << '\n';
			std::cout << '\n';

			std::cout << "rigid_body_circle_contact_constraint_spawner_old_death_indices:\n";
			for (GLuint index{ 0u }; index < rigid_body_circle_contact_constraint_spawner_old_write_count; ++index)
			{
				GLuint death_index;
				std::memcpy
				(
					&death_index,
					rigid_body_circle_contact_constraint_spawner_old_death_indices +
					index * environment.state.layouts.uint_data.state.array_stride,
					sizeof(GLuint)
				);
				std::cout
					<< "index: " << index
					<< ", death_index: " << death_index
					<< '\n';
			}
			std::cout << '\n';

			std::cout << "old_killed_rigid_body_circle_contact_constraint_spawners:\n";
			for (GLuint kill_item{ 0u }; kill_item < rigid_body_circle_contact_constraint_spawner_old_kill_count; ++kill_item)
			{
				GLuint killed;
				std::memcpy
				(
					&killed,
					old_killed_rigid_body_circle_contact_constraint_spawners +
					kill_item * environment.state.layouts.uint_data.state.array_stride,
					sizeof(GLuint)
				);
				GLuint death_index;
				std::memcpy
				(
					&death_index,
					rigid_body_circle_contact_constraint_spawner_old_death_indices +
					killed * environment.state.layouts.uint_data.state.array_stride,
					sizeof(GLuint)
				);

				std::cout
					<< "kill_item: " << kill_item
					<< ", killed: " << killed
					<< ", death_index: " << death_index
					<< '\n';
			}

			std::cout << std::endl;

			delete[] old_killed_rigid_body_circle_contact_constraint_spawners;
			delete[] rigid_body_circle_contact_constraint_spawner_old_death_indices;
		}

		delete[] fixed_data;
	}
}
