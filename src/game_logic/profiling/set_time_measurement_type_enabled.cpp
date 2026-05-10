#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"

namespace game_logic::profiling
{
	void set_time_measurement_type_enabled(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type, bool enabled)
	{
		if (environment.state.profiling.time_measurement == nullptr && enabled == true)
		{
			initialize_time_measurement_state(environment);
		}

		if (environment.state.profiling.time_measurement->measured_types[type] == enabled)
		{
			return;
		}

		environment.state.profiling.time_measurement->measured_type_count += static_cast<GLuint>(2 * static_cast<GLint>(enabled) - 1);
		environment.state.profiling.time_measurement->measured_types[type] = enabled;

		if (environment.state.profiling.time_measurement->measured_type_count == 0u)
		{
			free_time_measurement_state(environment);
		}
	}
}
