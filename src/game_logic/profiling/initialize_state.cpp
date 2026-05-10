#include "game_state/profiling/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void initialize_state(::game_environment::Environment& environment)
	{
		environment.state.profiling.time_measurement = nullptr;
	}
}
