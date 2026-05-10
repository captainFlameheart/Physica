#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void flush_timestamps(::game_environment::Environment& environment)
	{
		if (environment.state.profiling.time_measurement == nullptr)
		{
			return;
		}
		
		for (GLuint i{ 0u }; i < environment.state.profiling.time_measurement->query_count; ++i)
		{
			environment.state.profiling.time_measurement->base_query = (environment.state.profiling.time_measurement->base_query + 1u) % environment.state.profiling.time_measurement->query_capacity;
			environment.state.profiling.time_measurement->timestamp_base = (environment.state.profiling.time_measurement->timestamp_base + 1u) % environment.state.profiling.time_measurement->timestamp_capacity;
			glGetQueryBufferObjectui64v(
				environment.state.profiling.time_measurement->queries[environment.state.profiling.time_measurement->base_query],
				environment.state.profiling.time_measurement->buffer,
				GL_QUERY_RESULT,
				environment.state.profiling.time_measurement->timestamp_base * ::game_state::profiling::timestamp_value_size
			);
		}

		environment.state.profiling.time_measurement->query_count = 0u;
	}
}
