#pragma once
#include "glad_glfw.h"
#include "Timing.h"
#include "Timing_Type.h"
#include <string>
#include <string_view>
#include <array>
using namespace std::string_view_literals;

namespace game_state::profiling
{
	struct Timing_Set
	{
		Timing* (timings[static_cast<GLuint>(Timing_Type::count)]);
	};
}
