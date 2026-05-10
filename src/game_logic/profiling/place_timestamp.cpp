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
		if (environment.state.profiling.time_measurement->measured_types[type])
		{
			glQueryCounter(environment.state.profiling.time_measurement->queries[environment.state.profiling.time_measurement->next], GL_TIMESTAMP);
			
			environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].type = type;
			environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].previous =
				(is_start ? null_uint : environment.state.profiling.time_measurement->previous_timestamps[type]);
			environment.state.profiling.time_measurement->timestamp_metadata[environment.state.profiling.time_measurement->next].name = name;
			
			environment.state.profiling.time_measurement->previous_timestamps[type] = environment.state.profiling.time_measurement->next;

			++environment.state.profiling.time_measurement->next;
		}
	}
}
