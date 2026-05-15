#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"

namespace game_logic::profiling
{
	void flush_timing(::game_environment::Environment& environment, GLuint type);
}
