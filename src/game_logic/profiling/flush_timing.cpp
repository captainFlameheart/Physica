#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void flush_timing(::game_environment::Environment& environment, ::game_state::profiling::Timing_Type type)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] == nullptr)
		{
			return;
		}
	
		GLuint timestamp_capacity
		{
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_capacity
		};

		GLuint generation
		{
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation - environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count
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

			GLbyte* metadata_mapping{ environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_mapping };
			// TODO: Write metadata through mapping.

			++next_timestamp;
		}

		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count = 0u;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count = 0u;
	}
}
