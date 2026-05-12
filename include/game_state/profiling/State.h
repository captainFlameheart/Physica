#pragma once
#include "glad_glfw.h"
#include "Time_Measurement_State.h"
#include "Timing_Set.h"

namespace game_state::profiling
{
	struct State
	{
		Time_Measurement_State* time_measurement;
		Timing_Set timing_set;
	};
}
