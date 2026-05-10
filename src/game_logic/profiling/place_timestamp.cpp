#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "global/include.h"

namespace game_logic::profiling
{
	void place_timestamp(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type, std::string name, bool is_start = false)
	{
		if (environment.state.profiling.time_measurement == nullptr)
		{
			return;
		}
		if (environment.state.profiling.time_measurement->query_count == environment.state.profiling.time_measurement->query_capacity)
		{
			std::cerr << "Timestamp query capacity exceeded!" << std::endl;
			return;
		}
		if (environment.state.profiling.time_measurement->measured_types[type])
		{
			GLuint next_query = (environment.state.profiling.time_measurement->base_query + environment.state.profiling.time_measurement->query_count) % environment.state.profiling.time_measurement->query_capacity;
			glQueryCounter(environment.state.profiling.time_measurement->queries[next_query], GL_TIMESTAMP);
			
			GLuint old_next = environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].next;
			environment.state.profiling.time_measurement->timestamp_metadata[old_next].previous = null_uint;
			if (environment.state.profiling.time_measurement->previous_timestamps[environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].type] == environment.state.profiling.time_measurement->next)
			{
				environment.state.profiling.time_measurement->previous_timestamps[environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].type] = null_uint;
			}

			environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].type = type;
			GLuint previous = is_start ? null_uint : environment.state.profiling.time_measurement->previous_timestamps[type];
			environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].previous = previous;
			environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].next = environment.state.profiling.time_measurement->next;
			environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].name = name;
			
			if (!is_start)
			{
				environment.state.profiling.time_measurement->timestamp_metadata[previous].next = environment.state.profiling.time_measurement->next;
			}
			environment.state.profiling.time_measurement->previous_timestamps[type] = environment.state.profiling.time_measurement->next;

			environment.state.profiling.time_measurement->next = ((environment.state.profiling.time_measurement->next + 1u) % environment.state.profiling.time_measurement->timestamp_capacity);
			++environment.state.profiling.time_measurement->query_count;
		}
	}
}
