#pragma once
#include "glad_glfw.h"
#include "Time_Measurement_State.h"

namespace game_state::profiling
{
	struct State
	{
		Time_Measurement_State* time_measurement;
	};
}
