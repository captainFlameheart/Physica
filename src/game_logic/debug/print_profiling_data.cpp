#include "game_logic/debug/include.h"

namespace game_logic::debug
{
	void print_profiling_data(game_environment::Environment& environment)
	{
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

		GLubyte* fixed_data = new GLubyte[environment.state.layouts.fixed_data.block_state.buffer_data_size];
		glGetNamedBufferSubData
		(
			environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
			0u, environment.state.layouts.fixed_data.block_state.buffer_data_size, fixed_data
		);

		GLubyte* capacities = fixed_data + environment.state.layouts.fixed_data.capacities_state.offset;

		GLuint inner_bounding_box_capacity;
		std::memcpy
		(
			&inner_bounding_box_capacity,
			capacities + static_cast<GLuint>(::game_state::entity_type_indices::bounding_volume_hierarchy::Indices::inner_bounding_box) * environment.state.layouts.fixed_data.capacities_state.array_stride,
			sizeof(GLuint)
		);
		--inner_bounding_box_capacity;	// TODO: REMOVE AT SOME POINT

		if (environment.state.is_profiling_bounding_volume_hierarchy_rotations)
		{
			GLuint inner_bounding_box_rotation_base;
			std::memcpy
			(
				&inner_bounding_box_rotation_base,
				fixed_data + environment.state.layouts.fixed_data.inner_bounding_box_rotation_base_state.offset,
				sizeof(GLuint)
			);

			GLubyte* inner_bounding_box_rotations = new GLubyte[inner_bounding_box_capacity * environment.state.layouts.uvec4_data.state.array_stride];
			glGetNamedBufferSubData
			(
				environment.state.buffers.GPU_only.buffers[environment.state.buffers.GPU_only.current],
				environment.state.layouts.uvec4_data.state.offset + inner_bounding_box_rotation_base * environment.state.layouts.uvec4_data.state.array_stride,
				inner_bounding_box_capacity * environment.state.layouts.uvec4_data.state.array_stride,
				inner_bounding_box_rotations
			);

			std::cout << "Inner Bounding Box Rotations:\n";
			for (GLuint i{ 0u }; i < inner_bounding_box_capacity; ++i)
			{
				GLuint rotation[4u];
				std::memcpy
				(
					&rotation,
					inner_bounding_box_rotations + i * environment.state.layouts.uvec4_data.state.array_stride,
					sizeof(GLuint[4u])
				);
				if (rotation[3u] == 1u)
				{
					std::cout << i << ": (" << rotation[0u] << ", " << rotation[1u] << ", " << rotation[2u] << ", " << rotation[3u] << ")\n";
				}
			}
			std::cout << std::endl;

			delete[] inner_bounding_box_rotations;
		}

		delete[] fixed_data;
	}
}