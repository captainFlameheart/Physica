#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"

namespace game_logic::profiling
{
	void place_fence(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type);
}
