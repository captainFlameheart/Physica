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
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->generation_query_count = 0u;
		environment.state.profiling.timing_set.timings[static_cast<GLuint>(type)]->query_count = 0u;

		// TODO: Flush metadata.
	}
}
