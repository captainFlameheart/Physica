#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"
#include <array>

namespace game_logic::profiling
{
	void enable_timing(::game_environment::Environment& environment, GLuint type)
	{
		if (environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] == nullptr)
		{
			std::cout << "Enable timing of type " << ::game_state::profiling::timing_type_names[static_cast<GLuint>(type)] << std::endl;

			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)] = new ::game_state::profiling::Timing();
			
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_capacity = 1000u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_stage_capacity = 1000u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_capacity = 1000u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_capacity = 1000u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_capacity = 1000u;

			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation = 0u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp = 0u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count = 0u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count = 0u;

			glCreateBuffers
			(
				2u,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffers
			);

			GLuint total_timestamp_capacity
			{
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_capacity +
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_capacity
			};
			glNamedBufferStorage
			(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffer,
				total_timestamp_capacity * ::game_state::profiling::timestamp_value_size,
				nullptr,
				0
			);

			GLuint total_metadata_capacity
			{
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_capacity +
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_stage_capacity
			};
			GLuint metadata_buffer_size{ total_metadata_capacity * ::game_state::profiling::timestamp_value_size };
			glNamedBufferStorage
			(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_buffer,
				metadata_buffer_size,
				nullptr,
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_mapping = (GLbyte*)
				glMapNamedBufferRange
				(
					environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_buffer,
					0,
					metadata_buffer_size,
					GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
				);
			for (GLuint fence_index{ 0u }; fence_index < ::game_state::profiling::metadata_fence_count; ++fence_index)
			{
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fences[fence_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fence_timestamps[fence_index] = 0u;
			}

			glCreateQueries
			(
				GL_TIMESTAMP,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_capacity,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->queries
			);
			glCreateQueries
			(
				GL_TIMESTAMP,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_capacity,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_queries
			);

			GLuint timestamp_capacity = environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_capacity;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata = new ::game_state::profiling::Timestamp_Metadata1[timestamp_capacity];
			
			GLuint generation_capacity = environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_capacity;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_start_timestamps = new GLuint[generation_capacity];
		}
	}
}
