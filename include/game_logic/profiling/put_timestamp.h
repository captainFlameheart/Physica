#pragma once
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include "game_state/profiling/include.h"

namespace game_logic::profiling
{
	void put_timestamp(::game_environment::Environment& environment, ::game_state::profiling::Timestamp_Type type, std::string name, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha = 1.0f);
}
