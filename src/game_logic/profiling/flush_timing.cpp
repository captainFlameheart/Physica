#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"
#include <algorithm>

namespace game_logic::profiling
{
	void flush_timing(::game_environment::Environment& environment, GLuint type)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] == nullptr)
		{
			return;
		}

		glBindBufferBase
		(
			GL_UNIFORM_BUFFER,
			::game_state::bindings::uniform::timing_configuration,
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_configuration_buffer
		);
	
		GLuint timestamp_capacity
		{
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_capacity
		};

		GLuint generation
		{
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_generation - environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count
		};
		GLuint generation_capacity{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_capacity };
		for (GLuint i{ 0u }; i < environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count; ++i)
		{
			glGetQueryBufferObjectui64v(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_queries[i],
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffer,
				GL_QUERY_RESULT,
				(timestamp_capacity + (generation % generation_capacity)) * ::game_state::profiling::timestamp_value_size
			);

			++generation;
		}

		GLuint next_timestamp
		{
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp - environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count
		};
		for (GLuint i{ 0u }; i < environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count; ++i)
		{
			glGetQueryBufferObjectui64v(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->queries[i],
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffer,
				GL_QUERY_RESULT,
				(next_timestamp % timestamp_capacity) * ::game_state::profiling::timestamp_value_size
			);

			++next_timestamp;
		}

		glMemoryBarrier(GL_QUERY_BUFFER_BARRIER_BIT);

		// Write metadata to addresses the GPU is no longer reading from.

		next_timestamp = environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp - environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count;
		GLuint next_metadata_fence_index{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index };
		GLuint total_metadata_capacity
		{
			timestamp_capacity +
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_stage_capacity
		};
		GLuint initial_timestamp_metadata_physical_index = next_timestamp % total_metadata_capacity;

		for (GLuint i{ 0u }; i < environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count; ++i)
		{
			GLuint next_timestamp_metadata_physical_index{ next_timestamp % total_metadata_capacity };

			GLuint fence_timestamp{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fence_timestamps[next_metadata_fence_index] };
			GLuint fenced_start_timestamp{ fence_timestamp - timestamp_capacity };
			GLuint fenced_start_timestamp_metadata_physical_index{ fenced_start_timestamp % total_metadata_capacity };
			
			while (next_timestamp_metadata_physical_index == fenced_start_timestamp_metadata_physical_index)
			{
				GLsync fence{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fences[next_metadata_fence_index] };
				GLenum fence_status{ glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
				while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
				{
					if (fence_status == GL_WAIT_FAILED)
					{
						std::cerr << "Failed to wait on timestamp fence!" << std::endl;
					}
					if (fence_status == GL_TIMEOUT_EXPIRED)
					{
						std::cout << "Had to wait on timestamp fence! Consider increasing capacities." << std::endl;
					}
					fence_status = glClientWaitSync(fence, 0u, 0u);
				}
				// Note: We intentionally do not delete the fence.

				++next_metadata_fence_index;

				fence_timestamp = environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fence_timestamps[next_metadata_fence_index];
				fenced_start_timestamp = fence_timestamp - timestamp_capacity;
				fenced_start_timestamp_metadata_physical_index = fenced_start_timestamp % total_metadata_capacity;
			}

			GLubyte* timestamp_metadata_mapping{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_mapping };
			timestamp_metadata_mapping += next_timestamp_metadata_physical_index * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride;
			
			std::memcpy
			(
				timestamp_metadata_mapping + environment.state.layouts.timestamp_metadata.metadata_color_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[next_timestamp_metadata_physical_index].color,
				sizeof(GLfloat[4u])
			);
			std::memcpy
			(
				timestamp_metadata_mapping + environment.state.layouts.timestamp_metadata.metadata_generation_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata[next_timestamp_metadata_physical_index].generation,
				sizeof(GLuint)
			);

			++next_timestamp;
		}
		
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count != 0u)
		{
			GLuint timestamp_metadata_offsets[]
			{
				environment.state.layouts.timestamp_metadata.metadata_color_state.offset,
				environment.state.layouts.timestamp_metadata.metadata_generation_state.offset,
				environment.state.layouts.timestamp_metadata.metadata_name_base_state.offset,
			};
			GLuint timestamp_metadata_block_offset{ *std::min_element(std::begin(timestamp_metadata_offsets), std::end(timestamp_metadata_offsets)) };	// TODO: Precompute.

			GLuint final_timestamp_metadata_physical_index{ next_timestamp % total_metadata_capacity };
			if (final_timestamp_metadata_physical_index <= initial_timestamp_metadata_physical_index)
			{
				{
					GLuint start{ timestamp_metadata_block_offset + initial_timestamp_metadata_physical_index * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride };
					GLuint end{ timestamp_metadata_block_offset + total_metadata_capacity * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride };

					glFlushMappedNamedBufferRange
					(
						environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_buffer, start, end - start
					);
				}
				{
					GLuint start{ timestamp_metadata_block_offset };
					GLuint end{ timestamp_metadata_block_offset + final_timestamp_metadata_physical_index * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride };

					glFlushMappedNamedBufferRange
					(
						environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_buffer, start, end - start
					);
				}
			}
			else
			{
				GLuint start{ timestamp_metadata_block_offset + initial_timestamp_metadata_physical_index * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride };
				GLuint end{ timestamp_metadata_block_offset + final_timestamp_metadata_physical_index * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride };

				glFlushMappedNamedBufferRange
				(
					environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_buffer, start, end - start
				);
			}
		}
		
		next_metadata_fence_index = environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index;

		GLsync fence{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fences[next_metadata_fence_index] };
		GLenum fence_status{ glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0u) };
		while (fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED)
		{
			if (fence_status == GL_WAIT_FAILED)
			{
				std::cerr << "Failed to wait on timestamp fence!" << std::endl;
			}
			if (fence_status == GL_TIMEOUT_EXPIRED)
			{
				std::cout << "Had to wait on timestamp fence! Consider increasing capacities." << std::endl;
			}
			fence_status = glClientWaitSync(fence, 0u, 0u);
		}
		glDeleteSync(fence);

		GLubyte* timing_metadata_mapping{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_metadata_mapping };
		GLuint timing_metadata_region_offset{ next_metadata_fence_index * ::game_logic::binding_util::align_uniform_block_size(environment, environment.state.layouts.timestamp_metadata.timing_metadata_block_state.buffer_data_size) };
		timing_metadata_mapping += timing_metadata_region_offset;

		std::memcpy
		(
			timing_metadata_mapping + environment.state.layouts.timestamp_metadata.generation_state.offset,
			&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_generation,
			sizeof(GLuint)
		);
		std::memcpy
		(
			timing_metadata_mapping + environment.state.layouts.timestamp_metadata.next_timestamp_state.offset,
			&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp,
			sizeof(GLuint)
		);
		glFlushMappedNamedBufferRange
		(
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_metadata_buffer,
			timing_metadata_region_offset,
			environment.state.layouts.timestamp_metadata.timing_metadata_block_state.buffer_data_size
		);
		glBindBufferRange
		(
			GL_UNIFORM_BUFFER,
			::game_state::bindings::uniform::timing_metadata,
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_metadata_buffer,
			timing_metadata_region_offset, environment.state.layouts.timestamp_metadata.timing_metadata_block_state.buffer_data_size
		);

		glBindBufferBase
		(
			GL_SHADER_STORAGE_BUFFER,
			::game_state::bindings::shader_storage::timestamp,
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffer
		);
		glBindBufferBase
		(
			GL_SHADER_STORAGE_BUFFER,
			::game_state::bindings::shader_storage::timestamp_metadata,
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_buffer
		);

		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count = 0u;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count = 0u;
/*
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		GLuint timestamp_metadata_buffer_size{ total_metadata_capacity * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride + environment.state.layouts.timestamp_metadata.metadata_color_state.offset };
		GLubyte* temp = new GLubyte[timestamp_metadata_buffer_size];
		glGetNamedBufferSubData
		(
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_buffer,
			0, timestamp_metadata_buffer_size,
			temp
		);

		for (GLuint i{ 0u }; i < 10u; ++i)
		{
			GLfloat color[4u];
			std::memcpy
			(
				&color,
				temp + i * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride + environment.state.layouts.timestamp_metadata.metadata_color_state.offset,
				sizeof(GLfloat[4u])
			);

			GLuint generation;
			std::memcpy
			(
				&generation,
				temp + i * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride + environment.state.layouts.timestamp_metadata.metadata_generation_state.offset,
				sizeof(GLuint)
			);

			std::cout << i << ": color: (" << color[0u] << ", " << color[1u] << ", " << color[2u] << ", " << color[3u] << "), " << "generation: " << generation << "\n";
		}
		std::cout << std::endl;

		delete[] temp;*/
	}
}
