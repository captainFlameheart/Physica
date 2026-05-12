#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"

namespace game_logic::profiling
{
	void enable_timing(::game_environment::Environment& environment, GLuint type);
}
