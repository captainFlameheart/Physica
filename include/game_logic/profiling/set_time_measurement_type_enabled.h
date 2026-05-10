#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"

namespace game_logic::profiling
{
	void set_time_measurement_type_enabled(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type, bool enabled);
}
