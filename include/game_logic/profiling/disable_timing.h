#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void disable_timing(::game_environment::Environment& environment, GLuint type);
}
