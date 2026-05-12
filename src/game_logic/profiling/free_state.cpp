#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void free_state(::game_environment::Environment& environment)
	{
		free_time_measurement_state(environment);
		for (GLuint timing_type{ 0u }; timing_type < static_cast<GLuint>(::game_state::profiling::Timing_Type::count); ++timing_type)
		{
			disable_timing(environment, timing_type);
		}
	}
}
