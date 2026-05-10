#include "game_logic/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void free_state(::game_environment::Environment& environment)
	{
		free_time_measurement_state(environment);
	}
}
