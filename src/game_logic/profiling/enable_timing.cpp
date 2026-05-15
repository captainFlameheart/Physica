#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"
#include <array>
#include "global/include.h"

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

			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_generation = 0u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp = 0u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count = 0u;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count = 0u;

			glCreateBuffers
			(
				std::size(environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffers),
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->buffers
			);

			GLubyte* timing_configuration = new GLubyte[environment.state.layouts.timestamp_metadata.timing_configuration_block_state.buffer_data_size];
			std::memcpy
			(
				timing_configuration + environment.state.layouts.timestamp_metadata.timestamp_capacity_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_capacity,
				sizeof(GLuint)
			);
			std::memcpy
			(
				timing_configuration + environment.state.layouts.timestamp_metadata.metadata_stage_capacity_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_stage_capacity,
				sizeof(GLuint)
			);
			std::memcpy
			(
				timing_configuration + environment.state.layouts.timestamp_metadata.generation_capacity_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_capacity,
				sizeof(GLuint)
			);
			std::memcpy
			(
				timing_configuration + environment.state.layouts.timestamp_metadata.query_capacity_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_capacity,
				sizeof(GLuint)
			);
			std::memcpy
			(
				timing_configuration + environment.state.layouts.timestamp_metadata.generation_query_capacity_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_capacity,
				sizeof(GLuint)
			);
			glNamedBufferStorage
			(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_configuration_buffer,
				environment.state.layouts.timestamp_metadata.timing_configuration_block_state.buffer_data_size,
				timing_configuration,
				0
			);
			delete[] timing_configuration;

			GLuint timing_metadata_buffer_size{
				::game_logic::binding_util::align_uniform_block_size
				(
					environment,
					environment.state.layouts.timestamp_metadata.timing_metadata_block_state.buffer_data_size
				) * ::game_state::profiling::metadata_fence_count
			};
			GLubyte* timing_metadata = new GLubyte[timing_metadata_buffer_size];
			std::memcpy
			(
				timing_metadata + environment.state.layouts.timestamp_metadata.generation_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_generation,
				sizeof(GLuint)
			);
			std::memcpy
			(
				timing_metadata + environment.state.layouts.timestamp_metadata.next_timestamp_state.offset,
				&environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_timestamp,
				sizeof(GLuint)
			);
			glNamedBufferStorage
			(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_metadata_buffer,
				timing_metadata_buffer_size,
				timing_metadata,
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);
			delete[] timing_metadata;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_metadata_mapping = (GLubyte*)
				glMapNamedBufferRange
				(
					environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timing_metadata_buffer,
					0,
					timing_metadata_buffer_size,
					GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
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
			GLuint metadata_buffer_size
			{
				environment.state.layouts.timestamp_metadata.metadata_color_state.offset +
				total_metadata_capacity * environment.state.layouts.timestamp_metadata.metadata_color_state.top_level_array_stride
			};
			glNamedBufferStorage
			(
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_buffer,
				metadata_buffer_size,
				nullptr,
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
			);
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_mapping = (GLubyte*)
				glMapNamedBufferRange
				(
					environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->timestamp_metadata_buffer,
					0,
					metadata_buffer_size,
					GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
				);
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->next_metadata_fence_index = 0u;
			for (GLuint fence_index{ 0u }; fence_index < ::game_state::profiling::metadata_fence_count; ++fence_index)
			{
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fences[fence_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata_fence_timestamps[fence_index] = null_uint;
			}

			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->queries = new GLuint[environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_capacity];
			glCreateQueries
			(
				GL_TIMESTAMP,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_capacity,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->queries
			);

			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_queries = new GLuint[environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_capacity];
			glCreateQueries
			(
				GL_TIMESTAMP,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_capacity,
				environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_queries
			);

			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->metadata = new ::game_state::profiling::Timestamp_Metadata1[total_timestamp_capacity];
			
			GLuint generation_capacity = environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_capacity;
			environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_start_timestamps = new GLuint[generation_capacity];
		}
	}
}
