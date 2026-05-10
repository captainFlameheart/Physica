#include "game_state/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void free_time_measurement_state(::game_environment::Environment& environment)
	{
		if (environment.state.profiling.time_measurement == nullptr)
		{
			return;
		}

		glDeleteBuffers(1u, &environment.state.profiling.time_measurement->buffer);
		delete[] environment.state.profiling.time_measurement->timestamp_metadata;

		delete environment.state.profiling.time_measurement;
		environment.state.profiling.time_measurement = nullptr;
	}
}
