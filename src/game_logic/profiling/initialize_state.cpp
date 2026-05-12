#include "game_state/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void initialize_state(::game_environment::Environment& environment)
	{
		environment.state.profiling.time_measurement = nullptr;
		for (GLuint timing_type{ 0u }; timing_type < static_cast<GLuint>(::game_state::profiling::Timing_Type::count); ++timing_type)
		{
			environment.state.profiling.timing_set.timings[timing_type] = nullptr;
		}
	}
}
