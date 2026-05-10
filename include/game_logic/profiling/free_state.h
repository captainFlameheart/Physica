#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void free_state(::game_environment::Environment& environment);
}
